#ifndef LIBRARY_STACK_VECTOR_H
#error "Direct inclusion of this file is not allowed, include stack_vector.h"
// For the sake of sane code completion.
#include <library/common/struct/stack_vector/stack_vector.h>
#endif

#include <library/common/struct/stack_vector/error.h>

#include <format>
#include <memory>
#include <utility>

namespace NCommon::NStruct {

    template<typename T, size_t Capacity>
    template<bool IsConst>
    TStackVector<T, Capacity>::TBasicIterator<IsConst>::TBasicIterator(
        typename TStackVector<T, Capacity>::template TBasicIterator<IsConst>::pointer ptr
    ) noexcept
        : ptr_(ptr)
    {}

    template<typename T, size_t Capacity>
    template<bool IsConst>
    template<bool Enabled, typename>
    TStackVector<T, Capacity>::TBasicIterator<IsConst>::TBasicIterator(
        const TBasicIterator<false>& other
    ) noexcept
        : ptr_(other.ptr_)
    {}

    template<typename T, size_t Capacity>
    template<bool IsConst>
    template<bool Enabled, typename>
    TStackVector<T, Capacity>::TBasicIterator<IsConst>::TBasicIterator(
        TBasicIterator<false>&& other
    ) noexcept
        : ptr_(other.ptr_)
    {}

    template<typename T, size_t Capacity>
    template<bool IsConst>
    typename TStackVector<T, Capacity>::template TBasicIterator<IsConst>::reference
    TStackVector<T, Capacity>::TBasicIterator<IsConst>::operator*() const noexcept {
        return *ptr_;
    }

    template<typename T, size_t Capacity>
    template<bool IsConst>
    typename TStackVector<T, Capacity>::template TBasicIterator<IsConst>::pointer
    TStackVector<T, Capacity>::TBasicIterator<IsConst>::operator->() const noexcept {
        return ptr_;
    }

    template<typename T, size_t Capacity>
    template<bool IsConst>
    typename TStackVector<T, Capacity>::template TBasicIterator<IsConst>::reference
    TStackVector<T, Capacity>::TBasicIterator<IsConst>::operator[](
        difference_type delta
    ) const noexcept {
        return *(ptr_ + delta);
    }

    template<typename T, size_t Capacity>
    template<bool IsConst>
    typename TStackVector<T, Capacity>::template TBasicIterator<IsConst>&
    TStackVector<T, Capacity>::TBasicIterator<IsConst>::operator++() noexcept {
        ++ptr_;
        return *this;
    }

    template<typename T, size_t Capacity>
    template<bool IsConst>
    typename TStackVector<T, Capacity>::template TBasicIterator<IsConst>
    TStackVector<T, Capacity>::TBasicIterator<IsConst>::operator++(int) noexcept {
        auto iterator_copy = *this;
        ++ptr_;
        return iterator_copy;
    }

    template<typename T, size_t Capacity>
    template<bool IsConst>
    typename TStackVector<T, Capacity>::template TBasicIterator<IsConst>&
    TStackVector<T, Capacity>::TBasicIterator<IsConst>::operator--() noexcept {
        --ptr_;
        return *this;
    }

    template<typename T, size_t Capacity>
    template<bool IsConst>
    typename TStackVector<T, Capacity>::template TBasicIterator<IsConst>
    TStackVector<T, Capacity>::TBasicIterator<IsConst>::operator--(int) noexcept {
        auto iterator_copy = *this;
        --ptr_;
        return iterator_copy;
    }

    template<typename T, size_t Capacity>
    template<bool IsConst>
    typename TStackVector<T, Capacity>::template TBasicIterator<IsConst>&
    TStackVector<T, Capacity>::TBasicIterator<IsConst>::operator+=(
        difference_type delta
    ) noexcept {
        ptr_ += delta;
        return *this;
    }

    template<typename T, size_t Capacity>
    template<bool IsConst>
    typename TStackVector<T, Capacity>::template TBasicIterator<IsConst>&
    TStackVector<T, Capacity>::TBasicIterator<IsConst>::operator-=(
        difference_type delta
    ) noexcept {
        ptr_ -= delta;
        return *this;
    }

    template<typename T, size_t Capacity>
    template<bool IsConst>
    typename TStackVector<T, Capacity>::template TBasicIterator<IsConst>
    TStackVector<T, Capacity>::TBasicIterator<IsConst>::operator+(
        difference_type delta
    ) const noexcept {
        return TBasicIterator(ptr_ + delta);
    }

    template<typename T, size_t Capacity>
    template<bool IsConst>
    typename TStackVector<T, Capacity>::template TBasicIterator<IsConst>
    TStackVector<T, Capacity>::TBasicIterator<IsConst>::operator-(
        difference_type delta
    ) const noexcept {
        return TBasicIterator(ptr_ - delta);
    }

    template<typename T, size_t Capacity>
    template<bool IsConst>
    template<bool OtherConst>
    bool TStackVector<T, Capacity>::TBasicIterator<IsConst>::operator==(
        const TBasicIterator<OtherConst>& other
    ) const noexcept {
        return ptr_ == other.ptr_;
    }

    template<typename T, size_t Capacity>
    template<bool IsConst>
    template<bool OtherConst>
    bool TStackVector<T, Capacity>::TBasicIterator<IsConst>::operator!=(
        const TBasicIterator<OtherConst>& other
    ) const noexcept {
        return ptr_ != other.ptr_;
    }

    template<typename T, size_t Capacity>
    template<bool IsConst>
    template<bool OtherConst>
    bool TStackVector<T, Capacity>::TBasicIterator<IsConst>::operator<(
        const TBasicIterator<OtherConst>& other
    ) const noexcept {
        return ptr_ < other.ptr_;
    }

    template<typename T, size_t Capacity>
    template<bool IsConst>
    template<bool OtherConst>
    bool TStackVector<T, Capacity>::TBasicIterator<IsConst>::operator>(
        const TBasicIterator<OtherConst>& other
    ) const noexcept {
        return ptr_ > other.ptr_;
    }

    template<typename T, size_t Capacity>
    template<bool IsConst>
    template<bool OtherConst>
    bool TStackVector<T, Capacity>::TBasicIterator<IsConst>::operator<=(
        const TBasicIterator<OtherConst>& other
    ) const noexcept {
        return ptr_ <= other.ptr_;
    }

    template<typename T, size_t Capacity>
    template<bool IsConst>
    template<bool OtherConst>
    bool TStackVector<T, Capacity>::TBasicIterator<IsConst>::operator>=(
        const TBasicIterator<OtherConst>& other
    ) const noexcept {
        return ptr_ >= other.ptr_;
    }

    template<typename T, size_t Capacity>
    template<bool IsConst>
    template<bool OtherConst>
    typename TStackVector<T, Capacity>::template TBasicIterator<IsConst>::difference_type
    TStackVector<T, Capacity>::TBasicIterator<IsConst>::operator-(
        const TBasicIterator<OtherConst>& other
    ) const noexcept {
        return ptr_ - other.ptr_;
    }

    template<typename T, size_t Capacity>
    TStackVector<T, Capacity>::TStackVector(
        std::initializer_list<T> init
    ) {
        if (init.size() > Capacity) {
            throw NError::TStackVectorCapacityExceeded(Capacity);
        }
        
        try {
            for (const auto& value : init) {
                push_back(value);
            }
        } catch (...) {
            clear();
            throw;
        }
    }

    template<typename T, size_t Capacity>
    TStackVector<T, Capacity>::TStackVector(
        const TStackVector& other
    ) {
        try {
            for (size_t i = 0; i < other.size_; ++i) {
                push_back(other[i]);
            }
        } catch (...) {
            clear();
            throw;
        }
    }

    template<typename T, size_t Capacity>
    TStackVector<T, Capacity>::TStackVector(
        TStackVector&& other
    ) noexcept(std::is_nothrow_move_constructible_v<T>) {
        if constexpr (std::is_nothrow_move_constructible_v<T>) {
            for (size_t i = 0; i < other.size_; ++i) {
                push_back(std::move(other[i]));
            }
        } else {
            try {
                for (size_t i = 0; i < other.size_; ++i) {
                    push_back(std::move(other[i]));
                }
            } catch (...) {
                clear();
                throw;
            }
        }

        other.clear();
    }

    template<typename T, size_t Capacity>
    TStackVector<T, Capacity>&
    TStackVector<T, Capacity>::operator=(
        const TStackVector& other
    ) {
        if (this == std::addressof(other)) {
            return *this;
        }

        clear();

        try {
            for (size_t i = 0; i < other.size_; ++i) {
                push_back(other[i]);
            }
        } catch (...) {
            clear();
            throw;
        }

        return *this;
    }

    template<typename T, size_t Capacity>
    TStackVector<T, Capacity>&
    TStackVector<T, Capacity>::operator=(
        TStackVector&& other
    ) noexcept(std::is_nothrow_move_constructible_v<T>) {
        if (this == std::addressof(other)) {
            return *this;
        }

        clear();

        if constexpr (std::is_nothrow_move_constructible_v<T>) {
            for (size_t i = 0; i < other.size_; ++i) {
                push_back(std::move(other[i]));
            }
        } else {
            try {
                for (size_t i = 0; i < other.size_; ++i) {
                    push_back(std::move(other[i]));
                }
            } catch (...) {
                clear();
                throw;
            }
        }

        other.clear();

        return *this;
    }

    template<typename T, size_t Capacity>
    TStackVector<T, Capacity>::~TStackVector() {
        clear();
    }

    template<typename T, size_t Capacity>
    typename TStackVector<T, Capacity>::TSize
    TStackVector<T, Capacity>::size() const noexcept {
        return size_;
    }

    template<typename T, size_t Capacity>
    constexpr typename TStackVector<T, Capacity>::TSize
    TStackVector<T, Capacity>::capacity() noexcept {
        return Capacity;
    }

    template<typename T, size_t Capacity>
    bool TStackVector<T, Capacity>::empty() const noexcept {
        return size_ == 0;
    }

    template<typename T, size_t Capacity>
    T& TStackVector<T, Capacity>::operator[](
        TSize index
    ) noexcept {
        return data()[index];
    }

    template<typename T, size_t Capacity>
    const T& TStackVector<T, Capacity>::operator[](
        TSize index
    ) const noexcept {
        return data()[index];
    }

    template<typename T, size_t Capacity>
    T& TStackVector<T, Capacity>::at(
        TSize index
    ) {
        if (index >= size_) {
            throw std::out_of_range(
                std::format(
                    "TStackVector::at (index: {}, size {})",
                    index,
                    size_
                )
            );
        }

        return data()[index];
    }

    template<typename T, size_t Capacity>
    const T& TStackVector<T, Capacity>::at(
        TSize index
    ) const {
        if (index >= size_) {
            throw std::out_of_range(
                std::format(
                    "TStackVector::at (index: {}, size {})",
                    index,
                    size_
                )
            );
        }

        return data()[index];
    }

    template<typename T, size_t Capacity>
    T& TStackVector<T, Capacity>::front() noexcept {
        return data()[0];
    }

    template<typename T, size_t Capacity>
    const T& TStackVector<T, Capacity>::front() const noexcept {
        return data()[0];
    }

    template<typename T, size_t Capacity>
    T& TStackVector<T, Capacity>::back() noexcept {
        return data()[size_ - 1];
    }

    template<typename T, size_t Capacity>
    const T& TStackVector<T, Capacity>::back() const noexcept {
        return data()[size_ - 1];
    }

    template<typename T, size_t Capacity>
    T* TStackVector<T, Capacity>::data() noexcept {
        return reinterpret_cast<T*>(storage_.data());
    }

    template<typename T, size_t Capacity>
    const T* TStackVector<T, Capacity>::data() const noexcept {
        return reinterpret_cast<const T*>(storage_.data());
    }

    template<typename T, size_t Capacity>
    void TStackVector<T, Capacity>::push_back(
        const T& value
    ) {
        if (size_ == Capacity) {
            throw NError::TStackVectorCapacityExceeded(Capacity);
        }

        void* place = storage_.data() + sizeof(T) * size_;

        new (place) T(value);
        ++size_;
    }

    template<typename T, size_t Capacity>
    void TStackVector<T, Capacity>::push_back(
        T&& value
    ) {
        if (size_ == Capacity) {
            throw NError::TStackVectorCapacityExceeded(Capacity);
        }

        void* place = storage_.data() + sizeof(T) * size_;

        new (place) T(std::move(value));
        ++size_;
    }

    template<typename T, size_t Capacity>
    template<typename... Args>
    T& TStackVector<T, Capacity>::emplace_back(
        Args&&... args
    ) {
        if (size_ == Capacity) {
            throw NError::TStackVectorCapacityExceeded(Capacity);
        }

        void* place = storage_.data() + sizeof(T) * size_;

        T* object = new (place) T(std::forward<Args>(args)...);
        ++size_;

        return *object;
    }

    template<typename T, size_t Capacity>
    void TStackVector<T, Capacity>::pop_back() {
        --size_;
        std::destroy_at(data() + size_);
    }

    template<typename T, size_t Capacity>
    void TStackVector<T, Capacity>::clear() noexcept {
        while (size_ > 0) {
            --size_;
            std::destroy_at(data() + size_);
        }
    }

    template<typename T, size_t Capacity>
    typename TStackVector<T, Capacity>::TIterator
    TStackVector<T, Capacity>::begin() noexcept {
        return TIterator(data());
    }

    template<typename T, size_t Capacity>
    typename TStackVector<T, Capacity>::TConstIterator
    TStackVector<T, Capacity>::begin() const noexcept {
        return cbegin();
    }

    template<typename T, size_t Capacity>
    typename TStackVector<T, Capacity>::TConstIterator
    TStackVector<T, Capacity>::cbegin() const noexcept {
        return TConstIterator(data());
    }

    template<typename T, size_t Capacity>
    typename TStackVector<T, Capacity>::TIterator
    TStackVector<T, Capacity>::end() noexcept {
        return TIterator(data() + size_);
    }

    template<typename T, size_t Capacity>
    typename TStackVector<T, Capacity>::TConstIterator
    TStackVector<T, Capacity>::end() const noexcept {
        return cend();
    }

    template<typename T, size_t Capacity>
    typename TStackVector<T, Capacity>::TConstIterator
    TStackVector<T, Capacity>::cend() const noexcept {
        return TConstIterator(data() + size_);
    }

    template<typename T, size_t Capacity>
    typename TStackVector<T, Capacity>::TReverseIterator
    TStackVector<T, Capacity>::rbegin() noexcept {
        return TReverseIterator(end());
    }

    template<typename T, size_t Capacity>
    typename TStackVector<T, Capacity>::TConstReverseIterator
    TStackVector<T, Capacity>::rbegin() const noexcept {
        return crbegin();
    }

    template<typename T, size_t Capacity>
    typename TStackVector<T, Capacity>::TConstReverseIterator
    TStackVector<T, Capacity>::crbegin() const noexcept {
        return TConstReverseIterator(cend());
    }

    template<typename T, size_t Capacity>
    typename TStackVector<T, Capacity>::TReverseIterator
    TStackVector<T, Capacity>::rend() noexcept {
        return TReverseIterator(begin());
    }

    template<typename T, size_t Capacity>
    typename TStackVector<T, Capacity>::TConstReverseIterator
    TStackVector<T, Capacity>::rend() const noexcept {
        return crend();
    }

    template<typename T, size_t Capacity>
    typename TStackVector<T, Capacity>::TConstReverseIterator
    TStackVector<T, Capacity>::crend() const noexcept {
        return TConstReverseIterator(cbegin());
    }

} // namespace NCommon::NStruct
