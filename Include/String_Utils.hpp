#ifndef EXERCISE_II_STRING_UTILS_HPP
#define EXERCISE_II_STRING_UTILS_HPP

#include <cstdint>

namespace Utils::String {
    /**
     * StringToInt64 - Converts a string to a signed integer of 64 bits with in the specified base
     * @param string The string to convert from
     * @param value_out A pointer to a variable where te result value (signed integer of 64 bits with specified base) will be stored
     * @param base The base which we are converting. The default is 10
     * @return Returns true on success, false otherwise
     */
    bool StringToInt64(char *string, int64_t *value_out, int base = 10);

    /**
   * AllocateAndCopyString - Allocates enough memory to hold a copy of the given string, copies it and returns it
   * @param string The string to copy
   * @return Returns a pointer to the newly allocated string on success,
   * terminates the program on failure (out of memory error).
   */
    char *AllocateAndCopyString(const char *string);
}

#endif //EXERCISE_II_STRING_UTILS_HPP
