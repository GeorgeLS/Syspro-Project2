#ifndef EXERCISE_II_ARRAY_HPP
#define EXERCISE_II_ARRAY_HPP

#include <cstddef>
#include <type_traits>
#include <initializer_list>
#include "Memory_Utils.hpp"

using namespace Utils;

namespace Types {
    template<typename T>
    class Array {
    public:
        Array() : array_{nullptr}, elements_n_{0U} {}

        Array(std::initializer_list<T> list) {
            this->Construct(list);
        }

        Array(const Array<T> &array) {
            this->Construct(array);
        }

        Array &operator=(const Array<T> &array) {
            this->Destroy();
            this->Construct(array);
            return *this;
        }

        Array(Array<T> &&array) noexcept
        : array_{nullptr}, elements_n_{0U} {
            this->Move(array);
        }

        Array &operator=(Array<T> &&array) noexcept {
            this->Move(array);
            return *this;
        }

        ~Array() { this->Destroy(); }

        T &operator[](size_t index) {
            return array_[index];
        }

        const T &operator[](size_t index) const {
            return array_[index];
        }

        [[gnu::always_inline]]
        inline T *C_Array() const {
            return array_;
        }

        [[gnu::always_inline]]
        inline size_t Size() const { return elements_n_; }

    private:
        void Construct(std::initializer_list<T> &list) {
            elements_n_ = list.size();
            array_ = Memory::malloc<T>(elements_n_);
            Memory::CopyAndConstruct(array_, list.begin(), list.end());
        }

        void Construct(const Array<T> &array) {
            elements_n_ = array.elements_n_;
            array_ = Memory::malloc<T>(elements_n_);
            Memory::CopyAndConstruct(array, array.array_, elements_n_);
            // The thing is that if we could use C++17 we could optimize this further
            // but University is stuck to gcc 5.4 for some reason...
//            if constexpr (std::is_trivially_copy_constructible<T>::value) {
//                MemoryUtils::memcpy(array_, array.array_, elements_n_);
//            } else {
//                MemoryUtils::CopyAndConstruct(array_, array.array_, elements_n_);
//            }
        }

        void Destroy() {
            for (size_t i = 0U; i != elements_n_; ++i) array_[i].~T();
            free(array_);
        }

        void Move(Array<T> &array) {
            std::swap(this->elements_n_, array.elements_n_);
            std::swap(this->array_, array.array_);
        }

        T *array_;
        size_t elements_n_{};
    };
}

#endif //EXERCISE_II_ARRAY_HPP
