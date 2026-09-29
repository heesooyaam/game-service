#pragma once

#include <array>
#include <cstddef>
#include <initializer_list>
#include <iterator>
#include <type_traits>

namespace NCommon::NStruct {

    template<typename T, size_t Capacity>
    class TStackVector {

        static_assert(
            Capacity > 0, 
            "TStackVector Capacity must be greater than zero"
        );

    private:

        template<bool IsConst>
        class TBasicIterator {
        public:
            using iterator_category = std::random_access_iterator_tag;
            using value_type = T;
            using difference_type = ptrdiff_t;

            using pointer = std::conditional_t<IsConst, const T*, T*>;
            using reference = std::conditional_t<IsConst, const T&, T&>;

            TBasicIterator() noexcept = default;
            explicit TBasicIterator(pointer ptr) noexcept;

            TBasicIterator(const TBasicIterator&) noexcept = default;
            TBasicIterator(TBasicIterator&&) noexcept = default;

            TBasicIterator& operator=(const TBasicIterator&) noexcept = default;
            TBasicIterator& operator=(TBasicIterator&&) noexcept = default;

            template<
                bool Enabled = IsConst,
                typename = std::enable_if_t<Enabled> 
            >
            TBasicIterator(const TBasicIterator<false>& other) noexcept;

            template<
                bool Enabled = IsConst,
                typename = std::enable_if_t<Enabled> 
            >
            TBasicIterator(TBasicIterator<false>&& other) noexcept;

            reference operator*() const noexcept;
            pointer operator->() const noexcept;
            reference operator[](difference_type delta) const noexcept;

            TBasicIterator& operator++() noexcept;
            TBasicIterator operator++(int) noexcept;

            TBasicIterator& operator--() noexcept;
            TBasicIterator operator--(int) noexcept;

            TBasicIterator& operator+=(difference_type delta) noexcept;
            TBasicIterator& operator-=(difference_type delta) noexcept;

            TBasicIterator operator+(difference_type delta) const noexcept;
            TBasicIterator operator-(difference_type delta) const noexcept;

            template<bool OtherConst>
            bool operator==(const TBasicIterator<OtherConst>& other) const noexcept;

            template<bool OtherConst>
            bool operator!=(const TBasicIterator<OtherConst>& other) const noexcept;

            template<bool OtherConst>
            bool operator<(const TBasicIterator<OtherConst>& other) const noexcept;

            template<bool OtherConst>
            bool operator>(const TBasicIterator<OtherConst>& other) const noexcept;

            template<bool OtherConst>
            bool operator<=(const TBasicIterator<OtherConst>& other) const noexcept;

            template<bool OtherConst>
            bool operator>=(const TBasicIterator<OtherConst>& other) const noexcept;

            template<bool OtherConst>
            std::strong_ordering operator<=>(const TBasicIterator<OtherConst>& other) const noexcept;

            template<bool OtherConst>
            difference_type operator-(const TBasicIterator<OtherConst>& other) const noexcept;

            friend TBasicIterator operator+(difference_type delta, const TBasicIterator& it) noexcept {
                return it + delta;
            }

        private:
            void move_forward() noexcept;
            void move_backward() noexcept;

            void move(difference_type delta) noexcept;
            
        private:
            template<bool>
            friend class TBasicIterator;

            pointer ptr_ = nullptr;
        };

    public:
        using TIterator = TBasicIterator<false>;
        using TConstIterator = TBasicIterator<true>;

        using TReverseIterator = std::reverse_iterator<TIterator>;
        using TConstReverseIterator = std::reverse_iterator<TConstIterator>;

    public:

        TStackVector() = default;
        TStackVector(std::initializer_list<T> init) requires std::is_copy_constructible_v<T>;

        TStackVector(const TStackVector& other) requires std::is_copy_constructible_v<T>;

        TStackVector(TStackVector&& other)
            noexcept(std::is_nothrow_move_constructible_v<T>) 
            requires std::is_move_constructible_v<T>;

        TStackVector& operator=(const TStackVector& other) requires std::is_copy_constructible_v<T>;

        TStackVector& operator=(TStackVector&& other)
            noexcept(std::is_nothrow_move_constructible_v<T>) 
            requires std::is_move_constructible_v<T>;

        ~TStackVector();

        size_t size() const noexcept;
        static constexpr size_t capacity() noexcept;
        bool empty() const noexcept;

        T& operator[](size_t index) noexcept;
        const T& operator[](size_t index) const noexcept;

        T& at(size_t index);
        const T& at(size_t index) const;

        T& front() noexcept;
        const T& front() const noexcept;

        T& back() noexcept;
        const T& back() const noexcept;

        T* data() noexcept;
        const T* data() const noexcept;

        void unchecked_push_back(const T& value);
        void unchecked_push_back(T&& value);

        void push_back(const T& value);
        void push_back(T&& value);

        template<typename... Args>
        T& emplace_back(Args&&... args);

        void pop_back();
        void clear() noexcept;

        TIterator begin() noexcept;
        TConstIterator begin() const noexcept;
        TConstIterator cbegin() const noexcept;

        TIterator end() noexcept;
        TConstIterator end() const noexcept;
        TConstIterator cend() const noexcept;

        TReverseIterator rbegin() noexcept;
        TConstReverseIterator rbegin() const noexcept;
        TConstReverseIterator crbegin() const noexcept;

        TReverseIterator rend() noexcept;
        TConstReverseIterator rend() const noexcept;
        TConstReverseIterator crend() const noexcept;

    private:
        alignas(T) std::array<std::byte, sizeof(T) * Capacity> storage_;        
        size_t size_ = 0;
    };

} // namespace NCommon::NStruct


#define LIBRARY_STACK_VECTOR_H
#include <library/common/struct/stack_vector/detail/stack_vector-inl.h>
#undef LIBRARY_STACK_VECTOR_H
