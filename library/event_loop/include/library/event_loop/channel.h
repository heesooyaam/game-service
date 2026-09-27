#pragma once

#include <cstdint>
#include <functional>

namespace NEventLoop {

    class TEventLoop;

    enum class ERegistrationState : std::uint8_t {
        UNREGISTERED = 0,
        REGISTERED
    };

    class TChannel {
    public:
        using TCallback = std::function<void()>;

        TChannel(TEventLoop& event_loop, std::int32_t fd);
        ~TChannel();

        TChannel(const TChannel&) = delete;
        TChannel& operator=(const TChannel&) = delete;

        TChannel(TChannel&&) = delete;
        TChannel& operator=(TChannel&&) = delete;

        std::int32_t fd() const noexcept;
        std::uint32_t events() const noexcept;

        bool is_registered() const noexcept;
        bool is_reading() const noexcept;
        bool is_writing() const noexcept;

        void set_read_callback(TCallback callback);
        void set_write_callback(TCallback callback);
        void set_error_callback(TCallback callback);
        void set_close_callback(TCallback callback);

        void enable_reading();
        void disable_reading();

        void enable_writing();
        void disable_writing();

        void disable_all();
        void remove();

    private:
        friend class TEventLoop;

        void handle_events(std::uint32_t received_events);

    private:
        TEventLoop& event_loop_;

        TCallback read_callback_;
        TCallback write_callback_;
        TCallback error_callback_;
        TCallback close_callback_;

        std::int32_t fd_ = -1;
        std::uint32_t events_ = 0;

        ERegistrationState registered_state_ = ERegistrationState::UNREGISTERED;
    };

} // namespace NEventLoop
