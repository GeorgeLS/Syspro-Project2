#ifndef EXERCISE_II_MEMORYUTILS_HPP
#define EXERCISE_II_MEMORYUTILS_HPP

#include <cstddef>
#include <cstdlib>
#include <cstring>
#include "Report_Utils.hpp"

using namespace Utils::Report;

namespace Utils {
    namespace Memory {
        /**
         * malloc - Allocates the number of elements of specified type
         * @tparam T The type to allocate memory for
         * @param elements The number of elements to allocate
         * @return A pointer to the allocated memory of the type specified on success,
         * Terminates the program on failure
         */
        template<typename T>
        [[gnu::warn_unused_result, gnu::always_inline]]
        inline T *malloc(size_t elements) {
            T *memory = (T *) std::malloc(elements * sizeof(T));
            if (!memory) Die("Out of memory!");
            return memory;
        }

        /**
         * CopyAndConstruct - Copies the specified number of elements of type T from source to dest with copy construction
         * @tparam T The type of element to copy
         * @param dest A pointer to a block of memory of T to copy to
         * @param source A pointer to a block of memory of T to copy from
         * @param elements The number of elements to copy
         * WARNING (gliontos): You should use this only if T has a custom copy constructor. Otherwise use memcpy which is faster
         */
        template<typename T>
        [[gnu::always_inline]]
        inline void CopyAndConstruct(T *dest, T *source, size_t elements) {
            for (size_t i = 0U; i != elements; ++i) {
                dest[i] = source[i];
            }
        }

        /**
         * CopyAndConstruct - Copies the specified number of elements of type T from source to dest with copy construction using iterators
         * @tparam T The type of element to copy
         * @tparam Iterator The iterator to use to copy the elements from
         * @param dest A pointer to a block of memory of T to copy to
         * @param start The starting point of the iterator
         * @param end The ending point of the iterator
         */
        template<typename T, typename Iterator>
        inline void CopyAndConstruct(T *dest, Iterator start, Iterator end) {
            size_t i = 0U;
            for (Iterator it = start; it != end; ++it, ++i) {
                dest[i] = *it;
            }
        }

        /**
         * mempcy - Copies the specified number of elements of type T from source to dest
         * @tparam T The type of element to copy
         * @param dest A pointer to a block of memory of T to copy to
         * @param source A pointer to a block of memory of T to copy from
         * @param elements The number of elements to copy
         * WARNING (gliontos): This is a fast copy of source to dest. You should only use this if T hasn't a trivial copy constructor
         */
        template<typename T>
        [[gnu::always_inline]]
        inline void memcpy(T *dest, T *source, size_t elements) {
            std::memcpy(dest, source, elements * sizeof(T));
        }
    }
}

#endif //EXERCISE_II_MEMORYUTILS_HPP
