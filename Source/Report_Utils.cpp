#include <iostream>
#include <unistd.h>
#include <cstring>
#include "../Include/Report_Utils.hpp"

static void __Report(const char *tag, const char *fmt, va_list args) {
    if (tag) {
        fprintf(stderr, "%s", tag);
    }
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

void Utils::Report::FileReport(int fd, const char *fmt, ...) {
	va_list args;
	va_start(args, fmt);
	char *contents;
	vasprintf(&contents, fmt, args);
	write(fd, contents, strlen(contents));
	va_end(args);
	free(contents);
}

void Utils::Report::ReportMessage(const char *fmt, ...) {
	va_list args;
	va_start(args, fmt);
	__Report(NULL, fmt, args);
}
