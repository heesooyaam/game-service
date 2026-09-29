#define NTEST_MAIN
#include <library/test_framework/test.h>

#include <library/event_loop/channel.h>
#include <library/event_loop/event_loop.h>

#include <sys/socket.h>
#include <unistd.h>

#include <chrono>
#include <condition_variable>
#include <cstdint>
#include <mutex>
#include <stdexcept>
#include <thread>
#include <vector>

namespace {

    using namespace std::chrono_literals;

    class TFdPair {
    public:
        TFdPair() {
            if (::socketpair(AF_UNIX, SOCK_STREAM, 0, fds_) == -1) {
                throw std::runtime_error("socketpair failed");
            }
        }

        ~TFdPair() {
            ::close(fds_[0]);
            ::close(fds_[1]);
        }

        int first() const noexcept {
            return fds_[0];
        }

        int second() const noexcept {
            return fds_[1];
        }

    private:
        int fds_[2] = {-1, -1};
    };

    class TLoopWatchdog {
    public:
        explicit TLoopWatchdog(NEventLoop::TEventLoop& event_loop)
            : event_loop_(event_loop)
            , thread_([this] {
                std::unique_lock lock(mutex_);
                if (!condition_.wait_for(lock, 2s, [this] {
                    return cancelled_;
                })) {
                    event_loop_.stop();
                }
            })
        {}

        ~TLoopWatchdog() {
            {
                std::lock_guard lock(mutex_);
                cancelled_ = true;
            }
            condition_.notify_one();
            thread_.join();
        }

    private:
        NEventLoop::TEventLoop& event_loop_;
        std::mutex mutex_;
        std::condition_variable condition_;
        bool cancelled_ = false;
        std::thread thread_;
    };

    void run_with_timeout(NEventLoop::TEventLoop& event_loop) {
        TLoopWatchdog watchdog(event_loop);
        event_loop.run();
    }

} // namespace

namespace NEventLoop::NTests {

    TEST_CASE(test_post_from_another_thread_wakes_event_loop) {
        TEventLoop event_loop;

        bool callback_called = false;
        std::thread::id callback_thread;
        const auto loop_thread = std::this_thread::get_id();

        std::thread producer([&] {
            std::this_thread::sleep_for(10ms);
            event_loop.post([&] {
                callback_called = true;
                callback_thread = std::this_thread::get_id();
                event_loop.stop();
            });
        });

        run_with_timeout(event_loop);
        producer.join();

        CHECK(callback_called);
        CHECK(callback_thread == loop_thread);
    }

    TEST_CASE(test_posted_callbacks_keep_order_and_can_post_again) {
        TEventLoop event_loop;
        std::vector<int> order;

        event_loop.post([&] {
            order.push_back(1);
            event_loop.post([&] {
                order.push_back(3);
                event_loop.stop();
            });
        });
        event_loop.post([&] {
            order.push_back(2);
        });

        run_with_timeout(event_loop);

        CHECK(order == std::vector<int>({1, 2, 3}));
    }

    TEST_CASE(test_channel_dispatches_read_event) {
        TEventLoop event_loop;
        TFdPair sockets;
        TChannel channel(event_loop, sockets.first());
        bool read_called = false;

        channel.set_read_callback([&] {
            char value = 0;
            const auto bytes_read = ::read(sockets.first(), &value, sizeof(value));
            CHECK(bytes_read == 1);
            CHECK(value == 'x');

            read_called = true;
            channel.remove();
            event_loop.stop();
        });
        channel.enable_reading();

        const char value = 'x';
        CHECK(::write(sockets.second(), &value, sizeof(value)) == 1);

        run_with_timeout(event_loop);

        CHECK(read_called);
        CHECK(!channel.is_registered());
    }

    TEST_CASE(test_channel_updates_write_interest) {
        TEventLoop event_loop;
        TFdPair sockets;
        TChannel channel(event_loop, sockets.first());
        bool write_called = false;

        channel.set_write_callback([&] {
            write_called = true;
            channel.disable_writing();
            event_loop.post([&] {
                channel.remove();
                event_loop.stop();
            });
        });

        channel.enable_reading();
        channel.enable_writing();

        CHECK(channel.is_registered());
        CHECK(channel.is_reading());
        CHECK(channel.is_writing());

        run_with_timeout(event_loop);

        CHECK(write_called);
        CHECK(!channel.is_writing());
        CHECK(!channel.is_registered());
    }

} // namespace NEventLoop::NTests
