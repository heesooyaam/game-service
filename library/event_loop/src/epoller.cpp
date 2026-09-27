#include <library/event_loop/channel.h>
#include <library/io/epoller.h>

#include <array>
#include <cassert>
#include <cerrno>
#include <sys/epoll.h>

namespace NEventLoop::NIO {

    TEpoller::TEpoller()
        : epoll_fd_(::epoll_create1(EPOLL_CLOEXEC))
    {
        if (!epoll_fd_.valid()) {
            throw 1;
        }
    }

    void TEpoller::add(const TChannel& channel) {
        assert(!channel.is_registered());

        epoll_event event{};
        event.events = channel.events();
        event.data.fd = channel.fd();

        if (::epoll_ctl(
                epoll_fd_.get(),
                EPOLL_CTL_ADD,
                channel.fd(),
                &event
            ) == -1) {
            throw 1;
        }
    }

    void TEpoller::modify(const TChannel& channel) {
        assert(channel.is_registered());

        epoll_event event{};
        event.events = channel.events();
        event.data.fd = channel.fd();

        if (::epoll_ctl(
                epoll_fd_.get(),
                EPOLL_CTL_MOD,
                channel.fd(),
                &event
            ) == -1) {
            throw 1;
        }
    }

    void TEpoller::remove(const TChannel& channel) {
        assert(channel.is_registered());

        if (::epoll_ctl(
                epoll_fd_.get(),
                EPOLL_CTL_DEL,
                channel.fd(),
                nullptr
            ) == -1) {
            throw 1;
        }
    }

    std::vector<TEvent> TEpoller::wait() {
        std::array<epoll_event, MAX_EVENTS> events{};
        std::int32_t count = 0;

        do {
            count = ::epoll_wait(
                epoll_fd_.get(),
                events.data(),
                static_cast<std::int32_t>(events.size()),
                -1
            );
        } while (count == -1 && errno == EINTR);

        if (count == -1) {
            throw 1;
        }

        std::vector<TEvent> result;
        result.reserve(count);

        for (std::int32_t i = 0; i < count; ++i) {
            result.push_back({
                events[i].data.fd,
                events[i].events
            });
        }

        return result;
    }

} // namespace NEventLoop::NIO
