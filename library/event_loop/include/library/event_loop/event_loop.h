#pragma once

#include <atomic>
#include <cstdint>
#include <functional>
#include <memory>
#include <mutex>
#include <thread>
#include <unordered_map>
#include <vector>

namespace NEventLoop {

    class TChannel;

    class TEventLoop {
    public:
        using TCallback = std::function<void()>;

        TEventLoop();
        ~TEventLoop();

        TEventLoop(const TEventLoop&) = delete;
        TEventLoop& operator=(const TEventLoop&) = delete;

        TEventLoop(TEventLoop&&) = delete;
        TEventLoop& operator=(TEventLoop&&) = delete;

        void run();
        void stop() noexcept;
        void post(TCallback callback);

        bool is_running() const noexcept;
        bool is_in_loop_thread() const noexcept;

        void add_channel(TChannel& channel);
        void update_channel(TChannel& channel);
        void remove_channel(TChannel& channel);

    private:
        friend class TChannel;

        void ensure_in_loop_thread() const;
        void control_channel(int operation, TChannel& channel);
        void handle_wakeup();
        void wakeup();
        void run_pending_callbacks();

        int epoll_fd_ = -1;
        int wakeup_fd_ = -1;
        std::unique_ptr<TChannel> wakeup_channel_;
        std::unordered_map<int, TChannel*> channels_;

        std::mutex callbacks_mutex_;
        std::vector<TCallback> pending_callbacks_;

        const std::thread::id loop_thread_id_;
        std::atomic<bool> running_ = false;
        std::atomic<bool> stop_requested_ = false;
    };

} // namespace NEventLoop
