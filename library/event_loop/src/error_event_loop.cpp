#include <library/event_loop/error_event_loop.h>

#include <format>
#include <string_view>

namespace NEventLoop::NError {

    TEventLoopError::TEventLoopError(std::string_view label, std::string_view msg)
        : std::runtime_error(
            std::format(
                "[{}]: {}", 
                label,
                msg
            )
        )
    {}

    TEpollCreateError::TEpollCreateError()
        : TEventLoopError(
            "EVENT LOOP ERROR",
            "Epoll Creat Error"            
        )
    {}

    TEpollAddError::TEpollAddError()
        : TEventLoopError(
            "EVENT LOOP ERROR",
            "Epoll Add Error"            
        )
    {}

    TEpollModifyError::TEpollModifyError()
        : TEventLoopError(
            "EVENT LOOP ERROR",
            "Epoll Modify Error"            
        )
    {}

    TEpollRemoveError::TEpollRemoveError()
        : TEventLoopError(
            "EVENT LOOP ERROR",
            "Epoll Remove Error"            
        )
    {}

    TEpollWaitEventError::TEpollWaitEventError(const int32_t error_code)
        : TEventLoopError(
            "EVENT LOOP ERROR",
            std::format(
                "error code - {}. {}",
                error_code,
                std::system_category().message(error_code)
            )         
        )
    {}

} // namespace NEventLoop::NError
