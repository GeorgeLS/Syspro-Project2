#include <iostream>
#include "../Include/Report_Utils.hpp"

static void __Report(const char *tag, const char *fmt, va_list args) {
    fprintf(stderr, "%s", tag);
    vfprintf(stderr, fmt, args);
    fputc('\n', stderr);
    va_end(args);
}

void Utils::Report::ReportError(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    __Report("[ERROR]: ", fmt, args);
}

void Utils::Report::ReportError(const char *fmt, va_list args) {
    __Report("[ERROR]: ", fmt, args);
}

void Utils::Report::Die(const char *fmt, ...) {
    va_list args;
    va_start(args, fmt);
    ReportError(fmt, args);
    exit(EXIT_FAILURE);
}