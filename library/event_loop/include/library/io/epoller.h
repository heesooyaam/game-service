#pragma once

#include <library/io/fd.h>

#include <cstdint>
#include <vector>

namespace NEventLoop {
    
    class TChannel;

}

namespace NEventLoop::NIO {

    struct TEvent {
        int32_t fd = -1;
        uint32_t events = 0;
    };

    class TEpoller {
    public:
        TEpoller();
        ~TEpoller() = default;

        TEpoller(const TEpoller&) = delete;
        TEpoller& operator=(const TEpoller&) = delete;

        TEpoller(TEpoller&&) noexcept = default;
        TEpoller& operator=(TEpoller&&) noexcept = default;

        void add(const TChannel& channel);
        void modify(const TChannel& channel);
        void remove(const TChannel& channel);

        std::vector<TEvent> wait();

    private:
        static constexpr size_t MAX_EVENTS = 64;
        TFd epoll_fd_;
    };
    
} // namespace NEventLoop::NIO
