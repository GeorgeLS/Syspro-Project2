#ifndef EXERCISE_II_UTILS_HPP
#define EXERCISE_II_UTILS_HPP

#include "Client_Parameters.hpp"
#include "Report_Utils.hpp"

#define FOREACH(iterable) \
    for (auto it = iterable.Begin(); it != iterable.End(); ++it)

namespace Utils {
    /**
     * Usage - Prints the usage of the program
     */
    [[noreturn]]
    void Usage();
}

#endif //EXERCISE_II_UTILS_HPP
