#include <library/io/error_io.h>
#include <library/io/event_fd.h>

#include <cerrno>
#include <exception>
#include <unistd.h>
#include <sys/eventfd.h>

namespace NEventLoop::NIO {

    TEvenTFdWrapper::TEvenTFdWrapper()
        : fd_(::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC))
    {
        if (!fd_.valid()) {
            throw NError::TEventFdCreateError();
        }
    }

    int32_t TEvenTFdWrapper::fd() const noexcept {
        return fd_.get();
    }

    void TEvenTFdWrapper::notify() noexcept {
        uint64_t value = 1;
        int64_t result;

        do {
            result = ::write(
                fd_.get(),
                &value,
                sizeof(value)
            );
        } while (result == -1 && errno == EINTR);

        if (result != static_cast<int64_t>(sizeof(value))) {
            std::terminate();
        }
    }

    void TEvenTFdWrapper::consume() noexcept {
        uint64_t value = 0;
        int64_t result;

        do {
            result = ::read(
                fd_.get(),
                &value,
                sizeof(value)
            );
        } while (result == -1 && errno == EINTR);

        if (result != static_cast<int64_t>(sizeof(value))) {
            std::terminate();
        }
    }

} // namespace NEventLoop::NIO
