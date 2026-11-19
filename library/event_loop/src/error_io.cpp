#include <library/io/error_io.h>

#include <format>
#include <string_view>

namespace NEventLoop::NIO::NError {

    TEventLoopIOError::TEventLoopIOError(std::string_view label, std::string_view msg)
        : std::runtime_error(
            std::format(
                "[{}]: {}", 
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

} // namespace NEventLoop::NIO::NError
