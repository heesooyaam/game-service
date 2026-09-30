#pragma once

#include <library/io/fd.h>

#include <cstddef>
#include <cstdint>
#include <string>

namespace NEventLoop::NIO {

    struct TSocketAddress {
        std::string ip;
        uint16_t port = 0;
    };

    class TSocket {
    public:
        TSocket();
        ~TSocket() = default;

        TSocket(const TSocket&) = delete;
        TSocket& operator=(const TSocket&) = delete;

        TSocket(TSocket&&) noexcept = default;
        TSocket& operator=(TSocket&&) noexcept = default;

        int32_t fd() const noexcept;

        void bind(const TSocketAddress& address);
        void listen();

        TSocket accept();

<<<<<<< HEAD
        ptrdiff_t recv(void* buffer, size_t size);
        ptrdiff_t send(const void* buffer, size_t size);
=======
        ssize_t recv(void* buffer, size_t size);
        ssize_t send(const void* buffer, size_t size);
>>>>>>> 193c91e (better)

        void shutdown();

    private:
        explicit TSocket(TFd&& fd);

    private:
        TFd fd_;
    };

} // namespace NEventLoop::NIO
