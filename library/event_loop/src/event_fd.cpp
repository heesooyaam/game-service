#include <library/io/event_fd.h>

#include <cerrno>
#include <unistd.h>
#include <sys/eventfd.h>

namespace NEventLoop::NIO {

    TEventFd::TEventFd()
        : fd_(::eventfd(0, EFD_NONBLOCK | EFD_CLOEXEC))
    {
        if (!fd_.valid()) {
            throw 1;
        }
    }

    int32_t TEventFd::fd() const noexcept {
        return fd_.get();
    }

    void TEventFd::notify() {
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
            throw 1;
        }
    }

    void TEventFd::consume() {
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
            throw 1;
        }
    }

} // namespace NEventLoop::NIO
