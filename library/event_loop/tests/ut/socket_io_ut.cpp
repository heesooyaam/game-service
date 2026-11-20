#define NTEST_MAIN

#include <library/test_framework/test.h>

#include <library/io/error_io.h>
#include <library/io/socket.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <array>
#include <cstdint>
#include <stdexcept>

namespace {

    int get_port(int fd) {
        sockaddr_in address{};
        socklen_t size = sizeof(address);

        if (
            ::getsockname(
                fd,
                reinterpret_cast<sockaddr*>(&address),
                &size
            ) == -1
        ) {
            throw std::runtime_error("getsockname failed");
        }

        return ntohs(address.sin_port);
    }

    int create_client() {
        const int fd = ::socket(AF_INET, SOCK_STREAM, 0);

        if (fd == -1) {
            throw std::runtime_error("socket failed");
        }

        return fd;
    }

    void connect_client(int fd, int port) {
        sockaddr_in address{};
        address.sin_family = AF_INET;
        address.sin_port = htons(static_cast<std::uint16_t>(port));

        if (
            ::inet_pton(
                AF_INET,
                "127.0.0.1",
                &address.sin_addr
            ) != 1
        ) {
            ::close(fd);
            throw std::runtime_error("inet_pton failed");
        }

        if (
            ::connect(
                fd,
                reinterpret_cast<const sockaddr*>(&address),
                sizeof(address)
            ) == -1
        ) {
            ::close(fd);
            throw std::runtime_error("connect failed");
        }
    }

}

namespace NEventLoop::NIO::NTests {

    TEST_CASE(test_server_socket_initial_state) {
        TServerSocket server;

        CHECK(server.fd() >= 0);
    }

    TEST_CASE(test_server_socket_bind_invalid_ip) {
        TServerSocket server;

        CHECK_THROWS_AS(
            server.bind({"256.256.256.256", 8080}),
            NError::TInvalidIpAddressError
        );
    }

    TEST_CASE(test_server_socket_bind_and_listen) {
        TServerSocket server;

        server.bind({"127.0.0.1", 0});
        server.listen();

        CHECK(server.fd() >= 0);
    }

    TEST_CASE(test_server_socket_accept_without_client) {
        TServerSocket server;

        server.bind({"127.0.0.1", 0});
        server.listen();

        const auto client = server.accept();

        CHECK(!client.has_value());
    }

    TEST_CASE(test_server_socket_accept) {
        TServerSocket server;

        server.bind({"127.0.0.1", 0});
        server.listen();

        const int port = get_port(server.fd());
        const int client_fd = create_client();

        connect_client(client_fd, port);

        const auto client = server.accept();

        CHECK(client.has_value());
        CHECK(client->fd() >= 0);

        ::close(client_fd);
    }

    TEST_CASE(test_socket_recv_without_data) {
        TServerSocket server;

        server.bind({"127.0.0.1", 0});
        server.listen();

        const int port = get_port(server.fd());
        const int client_fd = create_client();

        connect_client(client_fd, port);

        auto socket = server.accept();
        CHECK(socket.has_value());

        std::array<std::byte, 3> buffer{};
        const TIOResult result = socket->recv(buffer);

        CHECK_EQ(result.status, EIOStatus::NOT_READY);
        CHECK_EQ(result.process_bytes, 0);

        ::close(client_fd);
    }

    TEST_CASE(test_socket_recv) {
        TServerSocket server;

        server.bind({"127.0.0.1", 0});
        server.listen();

        const int port = get_port(server.fd());
        const int client_fd = create_client();

        connect_client(client_fd, port);

        auto socket = server.accept();
        CHECK(socket.has_value());

        const std::array<std::byte, 3> sent = {
            std::byte{'a'},
            std::byte{'b'},
            std::byte{'c'}
        };

        CHECK_EQ(
            ::send(
                client_fd,
                sent.data(),
                sent.size(),
                0
            ),
            3
        );

        std::array<std::byte, 3> received{};
        const TIOResult result = socket->recv(received);

        CHECK_EQ(result.status, EIOStatus::SUCCESS);
        CHECK_EQ(result.process_bytes, 3);
        CHECK(received == sent);

        ::close(client_fd);
    }

    TEST_CASE(test_socket_send) {
        TServerSocket server;

        server.bind({"127.0.0.1", 0});
        server.listen();

        const int port = get_port(server.fd());
        const int client_fd = create_client();

        connect_client(client_fd, port);

        auto socket = server.accept();
        CHECK(socket.has_value());

        const std::array<std::byte, 3> sent = {
            std::byte{'x'},
            std::byte{'y'},
            std::byte{'z'}
        };

        const TIOResult result = socket->send(sent);

        CHECK_EQ(result.status, EIOStatus::SUCCESS);
        CHECK_EQ(result.process_bytes, 3);

        std::array<std::byte, 3> received{};
        CHECK_EQ(
            ::recv(
                client_fd,
                received.data(),
                received.size(),
                0
            ),
            3
        );

        CHECK(received == sent);

        ::close(client_fd);
    }

} // namespace NEventLoop::NIO::NTests
