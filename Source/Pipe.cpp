#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <iostream>
#include "../Include/Pipe.hpp"
#include "../Include/String_Utils.hpp"
#include "../Include/File_Utils.hpp"

using namespace String;
using namespace File;

Wrappers::Pipe::Pipe(const char *path, const size_t buffer_size)
        : buffer_(buffer_size),
          path_{AllocateAndCopyString(path)},
          fd_{-1} {}

bool Wrappers::Pipe::Open(Mode mode) {
    if (!FileExists(path_)) {
        if (mkfifo(path_, 0777) == -1) return false;
    }
    fd_ = open(path_, mode);
    return fd_ != -1;
}

void Wrappers::Pipe::Read(size_t bytes) {
    if (bytes > buffer_.Size()) {
        bytes = buffer_.Size();
    }
    read(fd_, buffer_.C_Array(), bytes);
}

void Wrappers::Pipe::Write(void *data, size_t bytes) {
    write(fd_, data, bytes);
}

bool Wrappers::Pipe::Close() {
    if (unlink(path_) == -1) return false;
    fd_ = -1;
    return true;
}

Wrappers::Pipe::~Pipe() {
    if (fd_ != -1) Close();
}

Wrappers::Pipe &Wrappers::Pipe::operator<<(const char *str) {
    Write((void *) str, strlen(str));
    return *this;
}

Wrappers::Pipe &Wrappers::Pipe::operator<<(unsigned long v) {
    Write(&v, sizeof(unsigned long));
    return *this;
}

Wrappers::Pipe &Wrappers::Pipe::operator>>(Array<char> &buffer) {
    read(fd_, buffer.C_Array(), buffer.Size());
    return *this;
}

Wrappers::Pipe &Wrappers::Pipe::operator>>(unsigned long &v) {
    read(fd_, &v, sizeof(unsigned long));
    return *this;
}
