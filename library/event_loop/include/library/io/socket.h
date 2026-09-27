#pragma once

#include <library/io/fd.h>

#include <cstddef>
#include <cstdint>
#include <string>

namespace NEventLoop::NIO {

    struct TSocketAddress {
        std::string ip;
        std::uint16_t port = 0;
    };

    class TSocket {
    public:
        TSocket();
        ~TSocket() = default;

        TSocket(const TSocket&) = delete;
        TSocket& operator=(const TSocket&) = delete;

        TSocket(TSocket&&) noexcept = default;
        TSocket& operator=(TSocket&&) noexcept = default;

        std::int32_t fd() const noexcept;

        void bind(const TSocketAddress& address);
        void listen();

        TSocket accept();

        std::ptrdiff_t recv(void* buffer, std::size_t size);
        std::ptrdiff_t send(const void* buffer, std::size_t size);

        void shutdown();

    private:
        explicit TSocket(TFd&& fd);

    private:
        TFd fd_;
    };

} // namespace NEventLoop::NIO
