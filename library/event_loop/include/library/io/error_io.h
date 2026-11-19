#pragma once

#include <stdexcept>
#include <string_view>

namespace NEventLoop::NIO::NError {

    class TEventLoopIOError : public std::runtime_error {
    protected:
        explicit TEventLoopIOError(std::string_view, std::string_view);
    };

    class TEventFdCreateError : public TEventLoopIOError {
    public:
        TEventFdCreateError();
    };

} // namespace NEventLoop::NIO::NError
