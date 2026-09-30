#pragma once

#include <cstdint>

namespace NEventLoop::NIO {

    class TFdWrapper {
    public:
        explicit TFdWrapper(int32_t fd) noexcept;
        ~TFdWrapper();

        TFdWrapper(const TFdWrapper&) = delete;
        TFdWrapper& operator=(const TFdWrapper&) = delete;

        TFdWrapper(TFdWrapper&& other) noexcept;
        TFdWrapper& operator=(TFdWrapper&& other) noexcept;

        int32_t get() const noexcept;
        bool valid() const noexcept;

        void close() noexcept;
    private:
        void reset() noexcept;
        
    private:
        int32_t fd_ = -1;
    };

} // namespace NEventLoop::NIO
