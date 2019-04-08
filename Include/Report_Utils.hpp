#ifndef EXERCISE_II_REPORTUTILS_HPP
#define EXERCISE_II_REPORTUTILS_HPP

#include <cstdarg>

namespace Utils::Report {
    /**
     * ReportError - Report an error printing to stderr with tag [ERROR]
     * @param fmt The string to print. It can include format specifiers
     * @param ... The rest of the arguments. These are provided in case the fmt string provided has format specifiers
     */
    [[gnu::format(printf, 1, 2)]]
    void ReportError(const char *fmt, ...);

    /**
     * ReportError - Report an error printing to stderr with tag [ERROR]
     * @param fmt The string to print. It can include format specifiers
     * @param args The rest of the arguments in the form of va_list. These are provided in case the fmt string provided has format specifiers
     */
    void ReportError(const char *fmt, va_list args);

    /**
     * FileReport - Report the given message to a file designated by the file descriptor
     * @param fd The file descriptor
     * @param fmt The string to print. It can include format specifiers
     * @param ... The rest of the arguments in the form of va_list. These are provided in case the fmt string provided has format specifiers
     */
    [[gnu::format(printf, 2, 3)]]
    void FileReport(int fd, const char *fmt, ...);

    [[gnu::format(printf, 1, 2)]]
    void ReportMessage(const char *fmt, ...);

    /**
     * Die - Prints an error message to stderr and terminates the program with EXIT_FAILURE
     * @param fmt The string to print. It can include format specifiers
     * @param ... The rest of the arguments. These are provided in case the fmt string provided has format specifiers
     */
    [[noreturn, gnu::format(printf, 1, 2)]]
    void Die(const char *fmt, ...);
}

#endif //EXERCISE_II_REPORTUTILS_HPP
