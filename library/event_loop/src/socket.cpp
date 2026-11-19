#include <library/io/socket.h>
#include <library/io/error_io.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>

#include <cassert>
#include <cerrno>
#include <utility>

namespace {

    int create_socket_or_throw() {
        const int socket_fd = ::socket(
            AF_INET, 
            SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, 
            0
        );
        if (socket_fd == -1) {
            throw NEventLoop::NIO::NError::TServerSocketCreateError(errno);
        }

        return socket_fd;
    }

}

//toDO: read about flags && errno
namespace NEventLoop::NIO {

    int32_t TSocket::fd() const noexcept {
        return fd_.get();
    }

    std::optional<size_t> TSocket::recv(std::span<std::byte> buffer) {
        const std::ptrdiff_t bytes_received = ::recv(
            fd_.get(),
            buffer.data(),
            buffer.size(),
            0
        );

        if (bytes_received >= 0) {
            return static_cast<size_t>(bytes_received);
        }

        if (
            errno == EINTR ||
            errno == EAGAIN ||
            errno == EWOULDBLOCK
        ) {
            return std::nullopt;
        }

        throw NError::TSocketRecvError(errno);
    }

    std::optional<size_t> TSocket::send(std::span<const std::byte> buffer) {
        const std::ptrdiff_t bytes_sent = ::send(
            fd_.get(),
            buffer.data(),
            buffer.size(),
            MSG_NOSIGNAL
        );

        if (bytes_sent >= 0) {
            return static_cast<size_t>(bytes_sent);
        }

        if (
            errno == EINTR ||
            errno == EAGAIN ||
            errno == EWOULDBLOCK
        ) {
            return std::nullopt;
        }

        throw NError::TSocketSendError(errno);
    }

    TSocket::TSocket(TFdWrapper&& fd)
        : fd_(std::move(fd))
    {}

    TServerSocket::TServerSocket()
        : fd_(
            create_socket_or_throw()
        ) 
    {
        assert(fd_.valid());   
    }
   
    int32_t TServerSocket::fd() const noexcept {
        return fd_.get();
    }

    void TServerSocket::bind(const TSocketAddress& address) {
        {
            const int option = 1;
            if (
                ::setsockopt(
                    fd_.get(),
                    SOL_SOCKET,
                    SO_REUSEADDR,
                    &option,
                    sizeof(option)
                ) == -1
            ) {
                throw NError::TSetSockoptError(errno);
            }
        }

        sockaddr_in socket_address{};
        socket_address.sin_family = AF_INET;
        socket_address.sin_port = htons(address.port);
        
        const int inet_pton_result = ::inet_pton(
            AF_INET,
            address.ip.c_str(),
            &socket_address.sin_addr
        );

        if (inet_pton_result == -1) {
            throw NError::TInetPtonError(errno);
        } 
        
        if (inet_pton_result == 0) {
            throw NError::TInvalidIpAddressError(address.ip);
        }
    
        if (
            ::bind(
                fd_.get(),
                reinterpret_cast<const sockaddr*>(&socket_address),
                sizeof(socket_address)
            ) == -1
        ) {
            throw NError::TBindError(errno);
        }
    }

    void TServerSocket::listen() {
        if (
            ::listen(
                fd_.get(), 
                SOMAXCONN
            ) == -1
        ) {
            throw NError::TListenError(errno);
        }
    }

    std::optional<TSocket> TServerSocket::accept() {
        const int32_t fd = ::accept4(
            fd_.get(),
            nullptr,
            nullptr,
            SOCK_NONBLOCK | SOCK_CLOEXEC
        );

        if (fd >= 0) {
            return TSocket(TFdWrapper(fd));
        }

        if (
            errno == EINTR ||
            errno == EAGAIN ||
            errno == EWOULDBLOCK)
        {
            return std::nullopt;
        }

        throw NError::TAcceptError(errno);
    }

} // namespace NEventLoop::NIO
