#pragma once

#include <array>
#include <cstddef>
#include <initializer_list>
#include <iterator>
#include <type_traits>

namespace NCommon {

    template<typename T, size_t Capacity>
    class TStackVector {
    public:

        TStackVector() noexcept = default;
        TStackVector(std::initializer_list<T> init);

        TStackVector(const TStackVector& other) = default;
        TStackVector(TStackVector&& other) noexcept = default;

        TStackVector& operator=(const TStackVector& other) = default;
        TStackVector& operator=(TStackVector&& other) noexcept = default;

        size_t size() const noexcept;
        constexpr size_t capacity() const noexcept;

    private:
        std::array<T, Capacity> storage_;
        size_t size_ = 0;

    private:
        
        template<bool IsConst>
        class TBasicIterator {
        public:
            using iterator_category = std::random_access_iterator_tag;
            using value_type = T;
            using difference_type = ptrdiff_t;

            using pointer   = std::conditional_t<IsConst, const T*, T*>;
            using reference = std::conditional_t<IsConst, const T&, T&>;

            TBasicIterator() noexcept = default;
            explicit TBasicIterator(pointer ptr) noexcept;

            TBasicIterator(const TBasicIterator& other) noexcept = default;
            TBasicIterator(TBasicIterator&& other) noexcept = default;

            TBasicIterator& operator=(const TBasicIterator& other) noexcept = default;
            TBasicIterator& operator=(TBasicIterator&& other) noexcept = default;

            template<bool IsConstIterator, typename = std::enable_if<IsConstIterator>>
            TBasicIterator(const TBasicIterator<false>& other) noexcept;     
            
            template<bool IsConstIterator, typename = std::enable_if<IsConstIterator>>
            TBasicIterator(TBasicIterator<false>&& other) noexcept;     

            reference operator*() const noexcept;
            pointer operator->() const noexcept;

            TBasicIterator& operator++() noexcept;
            TBasicIterator operator++(int) noexcept;

            TBasicIterator& operator--() noexcept;
            TBasicIterator operator--(int) noexcept;

            TBasicIterator& operator+=(difference_type delta) noexcept;
            TBasicIterator& operator-=(difference_type delta) noexcept;

            TBasicIterator operator+(difference_type delta) const noexcept;
            TBasicIterator operator-(difference_type delta) const noexcept;

            bool operator==(const TBasicIterator& other) const noexcept;
            bool operator!=(const TBasicIterator& other) const noexcept;
            bool operator<(const TBasicIterator& other) const noexcept;
            bool operator>(const TBasicIterator& other) const noexcept;
            bool operator<=(const TBasicIterator& other) const noexcept;
            bool operator>=(const TBasicIterator& other) const noexcept;

            difference_type operator-(const TBasicIterator& other) const noexcept;

        private:
            pointer ptr_ = nullptr;
        };
    
    public:
        using TIterator = TBasicIterator<false>;
        using TConstIterator = TBasicIterator<true>;
        using TReverseIterator = std::reverse_iterator<TIterator>;
        using TConstReverseIterator = std::reverse_iterator<TConstIterator>;
    
    };

} // namespace NCommon
