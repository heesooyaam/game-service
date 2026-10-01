#include <library/io/fd.h>

#include <memory>
#include <unistd.h>

namespace NEventLoop::NIO {

    TFdWrapper::TFdWrapper(int32_t fd) noexcept 
        : fd_(fd)
    {}

    TFdWrapper::~TFdWrapper() {
        close();
    }

    TFdWrapper::TFdWrapper(TFdWrapper&& other) noexcept
        : fd_(other.fd_)
    {
        other.reset();
    }

    TFdWrapper& TFdWrapper::operator=(TFdWrapper&& other) noexcept {
        if (this == std::addressof(other)) {
            return *this;
        }

        close();
        fd_ = other.fd_;
        other.reset();
        return *this;
    }

    int32_t TFdWrapper::get() const noexcept {
        return fd_;
    }

    bool TFdWrapper::valid() const noexcept {
        return fd_ != -1;
    }

    void TFdWrapper::close() noexcept {
        if (valid()) {
            ::close(fd_);
            reset();
        }
    }

    void TFdWrapper::reset() noexcept {
        fd_ = -1;
    }

} // namespace NEventLoop::NIO
