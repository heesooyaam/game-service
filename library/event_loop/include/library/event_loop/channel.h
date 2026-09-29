#pragma once

#include <cstdint>
#include <functional>

namespace NEventLoop {

    class TEventLoop;

    class TChannel {
    public:
        using TCallback = std::function<void()>;

        TChannel(TEventLoop& event_loop, int fd);
        ~TChannel();

        TChannel(const TChannel&) = delete;
        TChannel& operator=(const TChannel&) = delete;

        TChannel(TChannel&&) = delete;
        TChannel& operator=(TChannel&&) = delete;

        int fd() const noexcept;
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

        void update();
        void handle_events(std::uint32_t received_events);

        TEventLoop& event_loop_;
        int fd_;
        std::uint32_t events_ = 0;
        bool registered_ = false;

        TCallback read_callback_;
        TCallback write_callback_;
        TCallback error_callback_;
        TCallback close_callback_;
    };

} // namespace NEventLoop
