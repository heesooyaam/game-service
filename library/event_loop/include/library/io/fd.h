#pragma once

#include <cstdint>

namespace NEventLoop::NIO {

    //owns fd in class
    class TFd {
    public:
        explicit TFd(int32_t fd) noexcept;
        ~TFd();

        TFd(const TFd&) = delete;
        TFd& operator=(const TFd&) = delete;

        TFd(TFd&& other) noexcept;
        TFd& operator=(TFd&& other) noexcept;

        int32_t get() const noexcept;
        bool valid() const noexcept;

        void close() noexcept;
    private:
        void reset() noexcept;
        
    private:
        int32_t fd_ = -1;
    };

} // namespace NEventLoop::NIO
