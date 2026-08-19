# Event loop

`event_loop` is the Linux I/O loop discussed for the networking layer. It is
deliberately small: timers, connections, parsers, thread pools, and coroutines
are outside this library.

## Components

- `TEventLoop` owns `epoll`, an `eventfd` wake-up descriptor, registered
  channels, and the queue used by `post()`.
- `TChannel` associates one file descriptor with an event mask and callbacks
  for read, write, error, and close events.

The subscription path is:

```text
TConnection -> TChannel -> TEventLoop -> epoll
```

Events travel in the opposite direction:

```text
epoll -> TEventLoop -> TChannel -> callback
```

## Basic usage

```cpp
#include <library/event_loop/channel.h>
#include <library/event_loop/event_loop.h>

NEventLoop::TEventLoop loop;
NEventLoop::TChannel channel(loop, socket_fd);

channel.set_read_callback([&] {
    // Read all currently available data without blocking.
});
channel.enable_reading();

loop.run();

channel.remove();
```

`enable_reading()` registers a new channel automatically. Later interest-mask
changes use `epoll_ctl(EPOLL_CTL_MOD)`. `remove()` unregisters the descriptor;
the caller still owns and closes the descriptor itself.

## Posting work

`post()` is safe to call from another thread. It stores the callback under a
short mutex and writes to `eventfd`, waking a loop blocked in `epoll_wait()`.
The loop first finishes the current batch of I/O events, swaps the shared queue
into a local vector, releases the mutex, and then runs callbacks in order.

```cpp
loop.post([connection] {
    connection->send_in_loop("game started");
});
```

Callbacks must be short and non-blocking. A long callback blocks every other
channel and posted callback owned by the same loop.

## Threading and lifetime rules

- Construct, configure, run, and destroy a loop in one owning thread.
- `post()` and `stop()` are the only cross-thread operations.
- Configure and remove channels in the owning loop thread.
- Remove a channel before destroying it or closing its descriptor.
- A channel does not own its descriptor and must not outlive its event loop.

The target is built only on Linux because it directly uses `epoll` and
`eventfd`.
