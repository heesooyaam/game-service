#define NTEST_MAIN

#include <library/test_framework/test.h>

#include <library/event_loop/channel.h>
#include <library/event_loop/error_event_loop.h>
#include <library/event_loop/event_loop.h>

#include <atomic>
#include <cstdint>
#include <fcntl.h>
#include <future>
#include <stdexcept>
#include <sys/epoll.h>
#include <thread>
#include <unistd.h>

namespace {

    struct TPipe {
        int32_t read_fd = -1;
        int32_t write_fd = -1;

        TPipe() {
            int fds[2];
            if (::pipe(fds) == -1) {
                throw std::runtime_error("pipe failed");
            }

            read_fd = fds[0];
            write_fd = fds[1];
        }

        ~TPipe() {
            if (read_fd != -1) {
                ::close(read_fd);
            }

            if (write_fd != -1) {
                ::close(write_fd);
            }
        }

        TPipe(const TPipe&) = delete;
        TPipe& operator=(const TPipe&) = delete;
    };

}

namespace NEventLoop::NTests {

    TEST_CASE(test_channel_initial_state) {
        TPipe pipe;
        TEventLoop event_loop;
        TChannel channel(event_loop, pipe.read_fd);

        CHECK_EQ(channel.fd(), pipe.read_fd);
        CHECK_EQ(channel.events(), 0);
        CHECK(!channel.is_registered());
        CHECK(!channel.is_reading());
        CHECK(!channel.is_writing());
    }

    TEST_CASE(test_channel_reading) {
        TPipe pipe;
        TEventLoop event_loop;
        TChannel channel(event_loop, pipe.read_fd);

        channel.enable_reading();

        CHECK(channel.is_registered());
        CHECK(channel.is_reading());
        CHECK(!channel.is_writing());
        CHECK_EQ(
            channel.events(),
            static_cast<uint32_t>(EPOLLIN | EPOLLRDHUP)
        );

        channel.disable_reading();

        CHECK(!channel.is_registered());
        CHECK(!channel.is_reading());
        CHECK(!channel.is_writing());
        CHECK_EQ(channel.events(), 0);
    }

    TEST_CASE(test_channel_writing) {
        TPipe pipe;
        TEventLoop event_loop;
        TChannel channel(event_loop, pipe.write_fd);

        channel.enable_writing();

        CHECK(channel.is_registered());
        CHECK(!channel.is_reading());
        CHECK(channel.is_writing());
        CHECK_EQ(channel.events(), static_cast<uint32_t>(EPOLLOUT));

        channel.disable_writing();

        CHECK(!channel.is_registered());
        CHECK(!channel.is_reading());
        CHECK(!channel.is_writing());
        CHECK_EQ(channel.events(), 0);
    }

    TEST_CASE(test_channel_reading_and_writing) {
        TPipe pipe;
        TEventLoop event_loop;
        TChannel channel(event_loop, pipe.read_fd);

        channel.enable_reading();
        channel.enable_writing();

        CHECK(channel.is_registered());
        CHECK(channel.is_reading());
        CHECK(channel.is_writing());
        CHECK_EQ(
            channel.events(),
            static_cast<uint32_t>(EPOLLIN | EPOLLRDHUP | EPOLLOUT)
        );

        channel.disable_writing();

        CHECK(channel.is_registered());
        CHECK(channel.is_reading());
        CHECK(!channel.is_writing());

        channel.disable_reading();

        CHECK(!channel.is_registered());
        CHECK_EQ(channel.events(), 0);
    }

    TEST_CASE(test_channel_remove) {
        TPipe pipe;
        TEventLoop event_loop;
        TChannel channel(event_loop, pipe.read_fd);

        channel.enable_reading();
        channel.remove();

        CHECK(!channel.is_registered());
        CHECK(channel.is_reading());
        CHECK_EQ(
            channel.events(),
            static_cast<uint32_t>(EPOLLIN | EPOLLRDHUP)
        );
    }

    TEST_CASE(test_read_callback) {
        TPipe pipe;
        TEventLoop event_loop;
        TChannel channel(event_loop, pipe.read_fd);

        std::atomic_bool called = false;
        std::atomic_int read_result = -1;
        std::atomic_char read_value = 0;

        channel.set_read_callback([&] {
            char value = 0;
            read_result = static_cast<int>(::read(
                pipe.read_fd,
                &value,
                sizeof(value)
            ));
            read_value = value;

            called = true;
            channel.remove();
            event_loop.stop();
        });

        channel.enable_reading();

        std::thread thread([&] {
            event_loop.run();
        });

        const char value = 'x';
        CHECK_EQ(::write(pipe.write_fd, &value, sizeof(value)), 1);

        thread.join();

        CHECK(called);
        CHECK_EQ(read_result.load(), 1);
        CHECK_EQ(read_value.load(), 'x');
        CHECK(!channel.is_registered());
    }

    TEST_CASE(test_write_callback) {
        TPipe pipe;
        TEventLoop event_loop;
        TChannel channel(event_loop, pipe.write_fd);

        std::atomic_bool called = false;

        channel.set_write_callback([&] {
            called = true;
            channel.remove();
            event_loop.stop();
        });

        channel.enable_writing();

        std::thread thread([&] {
            event_loop.run();
        });

        thread.join();

        CHECK(called);
        CHECK(!channel.is_registered());
    }

    TEST_CASE(test_channel_add_exception_rolls_back_state) {
        const int fd = ::open("/dev/null", O_RDONLY | O_CLOEXEC);
        if (fd == -1) {
            throw std::runtime_error("open failed");
        }

        TEventLoop event_loop;
        TChannel channel(event_loop, fd);

        CHECK_THROWS_AS(
            channel.enable_reading(),
            NError::TEpollAddError
        );

        CHECK_EQ(channel.events(), 0);
        CHECK(!channel.is_registered());

        ::close(fd);
    }

    TEST_CASE(test_add_task) {
        TEventLoop event_loop;
        bool executed = false;

        event_loop.add_task([&] {
            executed = true;
            event_loop.stop();
        });

        event_loop.run();

        CHECK(executed);
    }

    TEST_CASE(test_add_tasks_order) {
        TEventLoop event_loop;
        std::vector<int> order;

        event_loop.add_task([&] {
            order.push_back(1);
        });

        event_loop.add_task([&] {
            order.push_back(2);
        });

        event_loop.add_task([&] {
            order.push_back(3);
            event_loop.stop();
        });

        event_loop.run();

        CHECK_EQ(order.size(), 3);
        CHECK_EQ(order[0], 1);
        CHECK_EQ(order[1], 2);
        CHECK_EQ(order[2], 3);
    }

    TEST_CASE(test_task_can_add_task) {
        TEventLoop event_loop;
        std::vector<int> order;

        event_loop.add_task([&] {
            order.push_back(1);

            event_loop.add_task([&] {
                order.push_back(2);
                event_loop.stop();
            });
        });

        event_loop.run();

        CHECK_EQ(order.size(), 2);
        CHECK_EQ(order[0], 1);
        CHECK_EQ(order[1], 2);
    }

    TEST_CASE(test_stop_from_another_thread) {
        TEventLoop event_loop;
        std::promise<void> started;
        auto started_future = started.get_future();

        std::thread thread([&] {
            event_loop.run();
        });

        event_loop.add_task([&] {
            started.set_value();
        });

        started_future.wait();
        event_loop.stop();

        thread.join();
    }

    TEST_CASE(test_add_task_from_another_thread) {
        TEventLoop event_loop;
        std::promise<void> executed;
        auto executed_future = executed.get_future();

        std::thread thread([&] {
            event_loop.run();
        });

        event_loop.add_task([&] {
            executed.set_value();
            event_loop.stop();
        });

        executed_future.wait();
        thread.join();
    }

    TEST_CASE(test_exception_from_task_keeps_event_loop_valid) {
        TEventLoop event_loop;

        event_loop.add_task([&] {
            throw std::runtime_error("test error");
        });

        CHECK_THROWS_AS(
            event_loop.run(),
            std::runtime_error
        );

        bool executed = false;

        event_loop.add_task([&] {
            executed = true;
            event_loop.stop();
        });

        event_loop.run();

        CHECK(executed);
    }

} // namespace NEventLoop::NTests
