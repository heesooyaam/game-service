#pragma once

#include <library/io/fd.h>

#include <cstdint>

namespace NEventLoop::NIO {

    class TEvenTFdWrapper {
    public:
        TEvenTFdWrapper();
        ~TEvenTFdWrapper() = default;

        TEvenTFdWrapper(const TEvenTFdWrapper&) = delete;
        TEvenTFdWrapper& operator=(const TEvenTFdWrapper&) = delete;

        TEvenTFdWrapper(TEvenTFdWrapper&&) noexcept = default;
        TEvenTFdWrapper& operator=(TEvenTFdWrapper&&) noexcept = default;

        int32_t fd() const noexcept;

        void notify() noexcept;
        void consume() noexcept;

    private:
        TFdWrapper fd_;
    };

} // namespace NEventLoop::NIO
