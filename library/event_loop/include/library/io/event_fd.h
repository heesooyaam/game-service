#pragma once

#include <library/io/fd.h>

#include <cstdint>

namespace NEventLoop::NIO {

    class TEventFdWrapper {
    public:
        TEventFdWrapper();
        ~TEventFdWrapper() = default;

        TEventFdWrapper(const TEventFdWrapper&) = delete;
        TEventFdWrapper& operator=(const TEventFdWrapper&) = delete;

        TEventFdWrapper(TEventFdWrapper&&) noexcept = default;
        TEventFdWrapper& operator=(TEventFdWrapper&&) noexcept = default;

        int32_t fd() const noexcept;

        void notify() noexcept;
        void consume() noexcept;

    private:
        TFdWrapper fd_;
    };

} // namespace NEventLoop::NIO
