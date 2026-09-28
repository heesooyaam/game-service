#include <library/event_loop/channel.h>
#include <library/event_loop/event_loop.h>

#include <cassert>
#include <sys/epoll.h>
#include <utility>

namespace NEventLoop {

    TChannel::TChannel(TEventLoop& event_loop, int32_t fd)
        : event_loop_(event_loop)
        , fd_(fd)
    {}

    TChannel::~TChannel() {
        assert(!is_registered());
    }

    int32_t TChannel::fd() const noexcept {
        return fd_;
    }

    uint32_t TChannel::events() const noexcept {
        return events_;
    }

    bool TChannel::is_registered() const noexcept {
        return registered_state_ == ERegistrationState::REGISTERED;
    }

    bool TChannel::is_reading() const noexcept {
        return events_ & EPOLLIN;
    }

    bool TChannel::is_writing() const noexcept {
        return events_ & EPOLLOUT;
    }

    void TChannel::set_read_callback(TChannel::TCallback callback) {
        read_callback_ = std::move(callback);
    }

    void TChannel::set_write_callback(TChannel::TCallback callback) {
        write_callback_ = std::move(callback);
    }

    void TChannel::set_error_callback(TChannel::TCallback callback) {
        error_callback_ = std::move(callback);
    }

    void TChannel::set_close_callback(TChannel::TCallback callback) {
        close_callback_ = std::move(callback);
    }

    void TChannel::enable_reading() {
        events_ |= EPOLLIN | EPOLLRDHUP;        
        event_loop_.update_channel(*this);
    }

    void TChannel::disable_reading() {
        events_ &= ~(EPOLLIN | EPOLLRDHUP);
        event_loop_.update_channel(*this);
    }

    void TChannel::enable_writing() {
        events_ |= EPOLLOUT;
        event_loop_.update_channel(*this);
    }

    void TChannel::disable_writing() {
        events_ &= ~EPOLLOUT;
        event_loop_.update_channel(*this);
    }

    void TChannel::disable_all() {
        events_ = 0;
        event_loop_.update_channel(*this);
    }

    void TChannel::remove() {
        event_loop_.remove_channel(*this);
    }
    
    void TChannel::handle_events(uint32_t received_events) {
        if ((received_events & EPOLLERR) != 0) {
            if (error_callback_) {
                error_callback_();
            }

            if (!is_registered()) {
                return;
            }
        }

        if ((received_events & EPOLLHUP) != 0 &&
            (received_events & EPOLLIN) == 0) {
            if (close_callback_) {
                close_callback_();
            }

            return;
        }

        if ((received_events & (EPOLLIN | EPOLLRDHUP)) != 0) {
            if (read_callback_) {
                read_callback_();
            }

            if (!is_registered()) {
                return;
            }
        }

        if ((received_events & EPOLLOUT) != 0) {
            if (write_callback_) {
                write_callback_();
            }
        }
    }

} // namespace NEventLoop
