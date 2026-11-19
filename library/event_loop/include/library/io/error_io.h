#pragma once

#include <stdexcept>
#include <string_view>

namespace NEventLoop::NIO::NError {

    class TNIOError : public std::runtime_error {
    protected:
        explicit TNIOError(std::string_view label, std::string_view msg);
    };

    class TEventLoopIOError : public TNIOError {
    protected:
        explicit TEventLoopIOError(std::string_view label, std::string_view msg);
    };

    class TEventFdCreateError : public TEventLoopIOError {
    public:
        TEventFdCreateError();
    };

    class TSocketError : public TNIOError {
    protected:
        explicit TSocketError(std::string_view label, std::string_view msg);
    };

    class TAcceptError : public TSocketError {
    public:
        TAcceptError(const int error_code);
    };

    class TListenError : public TSocketError {
    public:
        TListenError(const int error_code);
    };

    class TBindError : public TSocketError {
    public:
        TBindError(const int error_code);
    };

    class TInetPtonError : public TSocketError {
    public:
        TInetPtonError(const int error_code);
    };

    class TInvalidIpAddressError : public TSocketError {
    public:
        TInvalidIpAddressError(std::string_view address);
    };

    class TSetSockoptError : public TSocketError {
    public:
        TSetSockoptError(const int error_code);
    };

    class TServerSocketCreateError : public TSocketError {
    public:
        TServerSocketCreateError(const int error_code);
    };

    class TSocketSendError : public TSocketError {
    public:
        TSocketSendError(const int error_code);
    };

    class TSocketRecvError : public TSocketError {
    public:
        TSocketRecvError(const int error_code);
    };

} // namespace NEventLoop::NIO::NError
