#pragma once

#include <library/io/epoller.h>
#include <library/io/event_fd.h>

#include <atomic>
#include <cstdint>
#include <functional>
#include <memory>
#include <mutex>
#include <unordered_map>
#include <vector>

namespace NEventLoop {

    class TChannel;

    class TEventLoop {
    public:
        using TPendingTask = std::function<void()>;

        TEventLoop();
        ~TEventLoop();

        TEventLoop(const TEventLoop&) = delete;
        TEventLoop& operator=(const TEventLoop&) = delete;

        TEventLoop(TEventLoop&&) = delete;
        TEventLoop& operator=(TEventLoop&&) = delete;

        void run();
        void stop();

        void add_task(TPendingTask task);

    private:
        friend class TChannel;

        void update_channel(TChannel& channel);
        void remove_channel(TChannel& channel);

        void process_pending_tasks();

    private:
        NIO::TEpoller poller_;
        NIO::TEventFd notifier_;

        std::unordered_map<int32_t, TChannel*> channels_;

        std::unique_ptr<TChannel> notify_channel_;

        std::vector<TPendingTask> pending_tasks_;
        std::mutex pending_tasks_mutex_;

        std::atomic_bool running_ = false;
    };

} // namespace NEventLoop
