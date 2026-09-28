#pragma once

#include <library/io/fd.h>

#include <cstdint>

namespace NEventLoop::NIO {

    class TEventFd {
    public:
        TEventFd();
        ~TEventFd() = default;

        TEventFd(const TEventFd&) = delete;
        TEventFd& operator=(const TEventFd&) = delete;

        TEventFd(TEventFd&&) noexcept = default;
        TEventFd& operator=(TEventFd&&) noexcept = default;

        int32_t fd() const noexcept;

        void notify();
        void consume();

    private:
        TFd fd_;
    };

} // namespace NEventLoop::NIO
