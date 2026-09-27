#include <library/io/fd.h>

#include <memory>
#include <unistd.h>

namespace NEventLoop::NIO {

    TFd::TFd(std::int32_t fd) noexcept 
        : fd_(fd)
    {}

    TFd::~TFd() {
        close();
    }

    TFd::TFd(TFd&& other) noexcept
        : fd_(other.fd_)
    {
        other.reset();
    }

    TFd& TFd::operator=(TFd&& other) noexcept {
        if (this == std::addressof(other)) {
            return *this;
        }

        close();
        fd_ = other.fd_;
        other.reset();
        return *this;
    }

    std::int32_t TFd::get() const noexcept {
        return fd_;
    }

    bool TFd::valid() const noexcept {
        return fd_ != -1;
    }

    void TFd::close() noexcept {
        if (valid()) {
            ::close(fd_);
            reset();
        }
    }

    void TFd::reset() noexcept {
        fd_ = -1;
    }

} // namespace NEventLoop::NIO
