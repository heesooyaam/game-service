#include <library/event_loop/channel.h>
#include <library/event_loop/event_loop.h>
#include <library/event_loop/error_event_loop.h>

#include <cassert>
#include <mutex>

namespace {

    class TRunningGuard {
    public:
        explicit TRunningGuard(std::atomic_bool& running)
            : running_(running)
        {}

        ~TRunningGuard() {
            running_ = false;
        }

    private:
        std::atomic_bool& running_;
    };

}

namespace NEventLoop {

    TEventLoop::TEventLoop() 
        : notify_channel_(
            std::make_unique<TChannel>(
                *this,
                notifier_.fd()
            )
        )
    {
        notify_channel_->set_read_callback([this]() {
            notifier_.consume();
        });

        notify_channel_->enable_reading();
    }

    TEventLoop::~TEventLoop() {
        assert(channels_.size() == 1);
        assert(channels_.contains(notifier_.fd()));

        notify_channel_->registered_state_ = ERegistrationState::NOT_REGISTERED;
        notify_channel_.reset();
    }

    void TEventLoop::run() {
        running_ = true;
        TRunningGuard guard(running_);

        while (running_) {
            auto all_events = poller_.wait();
            for (const auto& [fd, events] : all_events) {
                const auto it = channels_.find(fd);
                if (it == channels_.end()) {
                    continue;
                }

                it->second.get().handle_events(events);
            }

            process_pending_tasks();
        }
    }

    void TEventLoop::stop() {
        running_ = false;
        notifier_.notify();
    }

    void TEventLoop::add_task(TPendingTask task) {
        {
            const std::lock_guard lock(pending_tasks_mutex_);
            pending_tasks_.emplace_back(std::move(task));
        }

        notifier_.notify();
    }

    void TEventLoop::update_channel(TChannel& channel, uint32_t prev_events) {
        if (channel.events() == 0) {
            if (channel.is_registered()) {
                try {
                    poller_.remove(channel);
                } catch (const NError::TEpollRemoveError&) {
                    channel.events_ = prev_events;
                    throw;
                } catch (...) {
                    assert(false);
                }

                channels_.erase(channel.fd());
                channel.registered_state_ = ERegistrationState::NOT_REGISTERED;
            }

            return;
        }

        if (!channel.is_registered()) {
            try {
                assert(channel.fd() != -1);

                auto [it, inserted] = channels_.emplace(
                    channel.fd(),
                    std::ref(channel)
                );

                assert(inserted);

                try {
                    poller_.add(channel);
                } catch (const NError::TEpollAddError&) {
                    channels_.erase(it);
                    channel.events_ = prev_events;
                    throw;
                } catch (...) {
                    assert(false);
                }

                channel.registered_state_ = ERegistrationState::REGISTERED;
            } catch (const std::bad_alloc&) {
                channel.events_ = prev_events;
                throw;
            }
            
        } else {

            try {
                poller_.modify(channel);
            } catch(const NError::TEpollModifyError&) {
                channel.events_ = prev_events;
                throw;
            } catch(...) {
                assert(false);
            }
        }
    }

    void TEventLoop::remove_channel(TChannel& channel) {
        if (channel.is_registered()) {
            poller_.remove(channel);
            channels_.erase(channel.fd());
            channel.registered_state_ = ERegistrationState::NOT_REGISTERED;
        }
    }

    void TEventLoop::process_pending_tasks() {
        std::vector<TPendingTask> tasks;

        {
            const std::lock_guard lock(pending_tasks_mutex_);
            tasks.swap(pending_tasks_);
        }

        for (auto& task : tasks) {
            task();
        }
    }

} // namespace NEventLoop
