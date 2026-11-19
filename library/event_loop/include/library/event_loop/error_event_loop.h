#pragma once

#include <stdexcept>
#include <string_view>

namespace NEventLoop::NError {

    class TEventLoopError : public std::runtime_error {
    protected:
        explicit TEventLoopError(std::string_view label, std::string_view msg);
    };

    class TEpollCreateError : public TEventLoopError {
    public:
        TEpollCreateError();
    };

    class TEpollAddError : public TEventLoopError {
    public:
        TEpollAddError();
    };

    class TEpollModifyError : public TEventLoopError {
    public:
        TEpollModifyError();
    };

    class TEpollRemoveError : public TEventLoopError {
    public:
        TEpollRemoveError();
    };

    class TEpollWaitEventError : public TEventLoopError {
    public:
        TEpollWaitEventError(const int error_code);
    };

} // namespace NEventLoop::NError
