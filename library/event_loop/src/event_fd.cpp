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

    std::int32_t TEventFd::fd() const noexcept {
        return fd_.get();
    }

    void TEventFd::notify() {
        std::uint64_t value = 1;
        std::int64_t result;

        do {
            result = ::write(
                fd_.get(),
                &value,
                sizeof(value)
            );
        } while (result == -1 && errno == EINTR);

        if (result != static_cast<std::int64_t>(sizeof(value))) {
            throw 1;
        }
    }

    void TEventFd::consume() {
        std::uint64_t value = 0;
        std::int64_t result;

        do {
            result = ::read(
                fd_.get(),
                &value,
                sizeof(value)
            );
        } while (result == -1 && errno == EINTR);

        if (result != static_cast<std::int64_t>(sizeof(value))) {
            throw 1;
        }
    }

} // namespace NEventLoop::NIO
