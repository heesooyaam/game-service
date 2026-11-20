#pragma once

#include <library/io/fd.h>

#include <cstddef>
#include <cstdint>
#include <optional>
#include <span>
#include <string>

namespace NEventLoop::NIO {

    struct TSocketAddress {
        std::string ip;
        std::uint16_t port = 0;
    };

    enum class EIOStatus : uint8_t {
        SUCCESS = 0,
        INTERRUPTED,
        NOT_READY,
        CLOSED
    };

    struct TIOResult {
        EIOStatus status = EIOStatus::NOT_READY;
        size_t process_bytes = 0;
    };

    class TServerSocket;

    class TSocket {
    public:
        ~TSocket() = default;

        TSocket(const TSocket&) = delete;
        TSocket& operator=(const TSocket&) = delete;

        TSocket(TSocket&&) noexcept = default;
        TSocket& operator=(TSocket&&) noexcept = default;

        int32_t fd() const noexcept;

        TIOResult recv(std::span<std::byte> buffer);
        TIOResult send(std::span<const std::byte> buffer);

    private:
        friend class TServerSocket;

        explicit TSocket(TFdWrapper&& fd);

    private:
        TFdWrapper fd_;
    };

    class TServerSocket {
    public:
        TServerSocket();
        ~TServerSocket() = default;

        TServerSocket(const TServerSocket&) = delete;
        TServerSocket& operator=(const TServerSocket&) = delete;

        TServerSocket(TServerSocket&&) noexcept = default;
        TServerSocket& operator=(TServerSocket&&) noexcept = default;

        int32_t fd() const noexcept;

        void bind(const TSocketAddress& address);
        void listen();

        std::optional<TSocket> accept();

    private:
        TFdWrapper fd_;
    };

} // namespace NEventLoop::NIO
