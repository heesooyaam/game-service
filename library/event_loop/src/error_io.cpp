#include <library/io/error_io.h>

#include <format>
#include <string_view>

namespace NEventLoop::NIO::NError {

    TNIOError::TNIOError(std::string_view label, std::string_view msg)
        : std::runtime_error(
            std::format(
                "[{}]: {}", 
                label,
                msg
            )
        )
    {}

    TEventLoopIOError::TEventLoopIOError(std::string_view label, std::string_view msg)
        : TNIOError(
            "TNIOError",
            std::format(
                "{} ( {} )", 
                label,
                msg
            )
        )
    {}

    TEventFdCreateError::TEventFdCreateError()
        : TEventLoopIOError(
            "EVENT LOOP IO ERROR",
            "Event Fd Creat Error"            
        )
    {}

    TSocketError::TSocketError(std::string_view label, std::string_view msg)
        : TNIOError(
            "TNIOError",
            std::format(
                "{} ( {} )", 
                label,
                msg
            )
        )
    {}

    TAcceptError::TAcceptError(const int error_code)
        : TSocketError(
            "ACCEPT ERROR",
            std::format(
                "error code - {}. {}",
                error_code,
                std::system_category().message(error_code)
            )         
        )
    {}

    TListenError::TListenError(const int error_code)
        : TSocketError(
            "LISTEN ERROR",
            std::format(
                "error code - {}. {}",
                error_code,
                std::system_category().message(error_code)
            )         
        )
    {}

    TBindError::TBindError(const int error_code)
        : TSocketError(
            "BIND ERROR",
            std::format(
                "error code - {}. {}",
                error_code,
                std::system_category().message(error_code)
            )         
        )
    {}

    TInetPtonError::TInetPtonError(const int error_code)
        : TSocketError(
            "INETPTON ERROR",
            std::format(
                "error code - {}. {}",
                error_code,
                std::system_category().message(error_code)
            )         
        )
    {}
    
    TInvalidIpAddressError::TInvalidIpAddressError(std::string_view address)
        : TSocketError(
            "INVALID IP ADDRESS",
            address
        )
    {}

    TSetSockoptError::TSetSockoptError(const int error_code)
        : TSocketError(
            "SETSOCKOPT ERROR",
            std::format(
                "error code - {}. {}",
                error_code,
                std::system_category().message(error_code)
            )         
        )
    {}

    TServerSocketCreateError::TServerSocketCreateError(const int error_code)
        : TSocketError(
            "CREATE SOCKET ERROR",
            std::format(
                "error code - {}. {}",
                error_code,
                std::system_category().message(error_code)
            )         
        )
    {}

    TSocketSendError::TSocketSendError(const int error_code)
        : TSocketError(
            "SOCKET SEND ERROR",
            std::format(
                "error code - {}. {}",
                error_code,
                std::system_category().message(error_code)
            )         
        )
    {}

    TSocketRecvError::TSocketRecvError(const int error_code)
        : TSocketError(
            "SOCKET RECV ERROR",
            std::format(
                "error code - {}. {}",
                error_code,
                std::system_category().message(error_code)
            )         
        )
    {}

} // namespace NEventLoop::NIO::NError
