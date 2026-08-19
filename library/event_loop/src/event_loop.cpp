#include <library/event_loop/event_loop.h>

#include <library/event_loop/channel.h>

#include <sys/epoll.h>
#include <sys/eventfd.h>
#include <unistd.h>

#include <array>
#include <cerrno>
#include <cassert>
#include <stdexcept>
#include <system_error>
#include <utility>

namespace {

    constexpr int MAX_EVENTS_PER_WAIT = 64;

    [[noreturn]] void throw_system_error(const char* operation) {
        throw std::system_error(errno, std::generic_category(), operation);
    }

} // namespace

namespace NEventLoop {

    TEventLoop::TEventLoop()
        : loop_thread_id_(std::this_thread::get_id())
    {
        epoll_fd_ = ::epoll_create1(EPOLL_CLOEXEC);
        if (epoll_fd_ == -1) {
            throw_system_error("epoll_create1");
        }

        wakeup_fd_ = ::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC);
        if (wakeup_fd_ == -1) {
            const int error = errno;
            ::close(epoll_fd_);
            epoll_fd_ = -1;
            throw std::system_error(error, std::generic_category(), "eventfd");
        }

        try {
            wakeup_channel_ = std::make_unique<TChannel>(*this, wakeup_fd_);
            wakeup_channel_->set_read_callback([this] {
                handle_wakeup();
            });
            wakeup_channel_->enable_reading();
        } catch (...) {
            ::close(wakeup_fd_);
            ::close(epoll_fd_);
            wakeup_fd_ = -1;
            epoll_fd_ = -1;
            throw;
        }
    }

    TEventLoop::~TEventLoop() {
        assert(!running_.load() && "event loop must be stopped before destruction");

        if (wakeup_channel_ && wakeup_channel_->registered_) {
            ::epoll_ctl(epoll_fd_, EPOLL_CTL_DEL, wakeup_fd_, nullptr);
            channels_.erase(wakeup_fd_);
            wakeup_channel_->registered_ = false;
        }
        wakeup_channel_.reset();

        for (auto& [fd, channel] : channels_) {
            static_cast<void>(fd);
            channel->registered_ = false;
        }
        channels_.clear();

        if (wakeup_fd_ != -1) {
            ::close(wakeup_fd_);
        }
        if (epoll_fd_ != -1) {
            ::close(epoll_fd_);
        }
    }

    void TEventLoop::run() {
        ensure_in_loop_thread();

        bool expected = false;
        if (!running_.compare_exchange_strong(expected, true)) {
            throw std::logic_error("event loop is already running");
        }

        try {
            std::array<epoll_event, MAX_EVENTS_PER_WAIT> events{};

            while (!stop_requested_.load()) {
                int event_count;
                do {
                    event_count = ::epoll_wait(
                        epoll_fd_,
                        events.data(),
                        static_cast<int>(events.size()),
                        -1
                    );
                } while (event_count == -1 && errno == EINTR && !stop_requested_.load());

                if (event_count == -1) {
                    if (errno == EINTR && stop_requested_.load()) {
                        break;
                    }
                    throw_system_error("epoll_wait");
                }

                for (int index = 0; index < event_count; ++index) {
                    const int fd = events[index].data.fd;
                    const auto channel = channels_.find(fd);
                    if (channel != channels_.end()) {
                        channel->second->handle_events(events[index].events);
                    }
                }

                run_pending_callbacks();
            }
        } catch (...) {
            running_.store(false);
            throw;
        }

        running_.store(false);
    }

    void TEventLoop::stop() noexcept {
        stop_requested_.store(true);
        try {
            wakeup();
        } catch (...) {
        }
    }

    void TEventLoop::post(TCallback callback) {
        if (!callback) {
            throw std::invalid_argument("posted callback must not be empty");
        }

        {
            std::lock_guard lock(callbacks_mutex_);
            pending_callbacks_.push_back(std::move(callback));
        }

        wakeup();
    }

    bool TEventLoop::is_running() const noexcept {
        return running_.load();
    }

    bool TEventLoop::is_in_loop_thread() const noexcept {
        return std::this_thread::get_id() == loop_thread_id_;
    }

    void TEventLoop::add_channel(TChannel& channel) {
        ensure_in_loop_thread();
        if (&channel.event_loop_ != this) {
            throw std::invalid_argument("channel belongs to another event loop");
        }
        if (channel.registered_) {
            throw std::logic_error("channel is already registered");
        }
        if (channel.events_ == 0) {
            throw std::logic_error("cannot add a channel without events");
        }

        const auto [iterator, inserted] = channels_.emplace(channel.fd_, &channel);
        if (!inserted) {
            throw std::logic_error("another channel already uses this fd");
        }

        try {
            control_channel(EPOLL_CTL_ADD, channel);
        } catch (...) {
            channels_.erase(iterator);
            throw;
        }
        channel.registered_ = true;
    }

    void TEventLoop::update_channel(TChannel& channel) {
        ensure_in_loop_thread();
        if (&channel.event_loop_ != this) {
            throw std::invalid_argument("channel belongs to another event loop");
        }

        const auto iterator = channels_.find(channel.fd_);
        if (!channel.registered_ || iterator == channels_.end() || iterator->second != &channel) {
            throw std::logic_error("channel is not registered in this event loop");
        }

        control_channel(EPOLL_CTL_MOD, channel);
    }

    void TEventLoop::remove_channel(TChannel& channel) {
        ensure_in_loop_thread();
        if (&channel.event_loop_ != this) {
            throw std::invalid_argument("channel belongs to another event loop");
        }

        const auto iterator = channels_.find(channel.fd_);
        if (!channel.registered_ || iterator == channels_.end() || iterator->second != &channel) {
            throw std::logic_error("channel is not registered in this event loop");
        }

        control_channel(EPOLL_CTL_DEL, channel);
        channels_.erase(iterator);
        channel.registered_ = false;
    }

    void TEventLoop::ensure_in_loop_thread() const {
        if (!is_in_loop_thread()) {
            throw std::logic_error("operation must run in the event loop thread");
        }
    }

    void TEventLoop::control_channel(int operation, TChannel& channel) {
        epoll_event event{};
        event.events = channel.events_;
        event.data.fd = channel.fd_;

        epoll_event* event_pointer = operation == EPOLL_CTL_DEL ? nullptr : &event;
        if (::epoll_ctl(epoll_fd_, operation, channel.fd_, event_pointer) == -1) {
            throw_system_error("epoll_ctl");
        }
    }

    void TEventLoop::handle_wakeup() {
        eventfd_t value;
        while (::eventfd_read(wakeup_fd_, &value) == -1) {
            if (errno == EINTR) {
                continue;
            }
            if (errno == EAGAIN) {
                return;
            }
            throw_system_error("eventfd_read");
        }
    }

    void TEventLoop::wakeup() {
        while (::eventfd_write(wakeup_fd_, 1) == -1) {
            if (errno == EINTR) {
                continue;
            }
            if (errno == EAGAIN) {
                return;
            }
            throw_system_error("eventfd_write");
        }
    }

    void TEventLoop::run_pending_callbacks() {
        std::vector<TCallback> callbacks;
        {
            std::lock_guard lock(callbacks_mutex_);
            callbacks.swap(pending_callbacks_);
        }

        for (auto& callback : callbacks) {
            callback();
        }
    }

} // namespace NEventLoop
