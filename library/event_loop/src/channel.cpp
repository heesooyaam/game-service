#include <library/event_loop/channel.h>

#include <library/event_loop/event_loop.h>

#include <sys/epoll.h>

#include <cassert>
#include <stdexcept>
#include <utility>

namespace {

    constexpr std::uint32_t READ_EVENTS = EPOLLIN | EPOLLPRI | EPOLLRDHUP;
    constexpr std::uint32_t WRITE_EVENTS = EPOLLOUT;

} // namespace

namespace NEventLoop {

    TChannel::TChannel(TEventLoop& event_loop, int fd)
        : event_loop_(event_loop)
        , fd_(fd)
    {
        if (fd_ < 0) {
            throw std::invalid_argument("channel fd must be non-negative");
        }
    }

    TChannel::~TChannel() {
        if (!registered_) {
            return;
        }

        try {
            event_loop_.remove_channel(*this);
        } catch (...) {
            assert(false && "registered channel must be destroyed in its event loop thread");
        }
    }

    int TChannel::fd() const noexcept {
        return fd_;
    }

    std::uint32_t TChannel::events() const noexcept {
        return events_;
    }

    bool TChannel::is_registered() const noexcept {
        return registered_;
    }

    bool TChannel::is_reading() const noexcept {
        return (events_ & READ_EVENTS) != 0;
    }

    bool TChannel::is_writing() const noexcept {
        return (events_ & WRITE_EVENTS) != 0;
    }

    void TChannel::set_read_callback(TCallback callback) {
        event_loop_.ensure_in_loop_thread();
        read_callback_ = std::move(callback);
    }

    void TChannel::set_write_callback(TCallback callback) {
        event_loop_.ensure_in_loop_thread();
        write_callback_ = std::move(callback);
    }

    void TChannel::set_error_callback(TCallback callback) {
        event_loop_.ensure_in_loop_thread();
        error_callback_ = std::move(callback);
    }

    void TChannel::set_close_callback(TCallback callback) {
        event_loop_.ensure_in_loop_thread();
        close_callback_ = std::move(callback);
    }

    void TChannel::enable_reading() {
        event_loop_.ensure_in_loop_thread();
        events_ |= READ_EVENTS;
        update();
    }

    void TChannel::disable_reading() {
        event_loop_.ensure_in_loop_thread();
        events_ &= ~READ_EVENTS;
        update();
    }

    void TChannel::enable_writing() {
        event_loop_.ensure_in_loop_thread();
        events_ |= WRITE_EVENTS;
        update();
    }

    void TChannel::disable_writing() {
        event_loop_.ensure_in_loop_thread();
        events_ &= ~WRITE_EVENTS;
        update();
    }

    void TChannel::disable_all() {
        event_loop_.ensure_in_loop_thread();
        events_ = 0;
        update();
    }

    void TChannel::remove() {
        event_loop_.ensure_in_loop_thread();
        if (registered_) {
            event_loop_.remove_channel(*this);
        }
    }

    void TChannel::update() {
        if (registered_) {
            event_loop_.update_channel(*this);
        } else if (events_ != 0) {
            event_loop_.add_channel(*this);
        }
    }

    void TChannel::handle_events(std::uint32_t received_events) {
        const bool should_close =
            (received_events & EPOLLHUP) != 0 && (received_events & EPOLLIN) == 0;
        const bool has_error = (received_events & EPOLLERR) != 0;
        const bool can_read = (received_events & READ_EVENTS) != 0;
        const bool can_write = (received_events & WRITE_EVENTS) != 0;

        const auto close_callback = close_callback_;
        const auto error_callback = error_callback_;
        const auto read_callback = read_callback_;
        const auto write_callback = write_callback_;

        if (should_close && close_callback) {
            close_callback();
        }
        if (has_error && error_callback) {
            error_callback();
        }
        if (can_read && read_callback) {
            read_callback();
        }
        if (can_write && write_callback) {
            write_callback();
        }
    }

} // namespace NEventLoop
