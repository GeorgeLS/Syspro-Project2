#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <iostream>
#include <poll.h>
#include "../Include/Pipe.hpp"
#include "../Include/String_Utils.hpp"
#include "../Include/File_Utils.hpp"

using namespace String;
using namespace File;

Wrappers::Pipe::Pipe(const char *path, const size_t buffer_size)
        : buffer_(buffer_size),
          path_{AllocateAndCopyString(path)},
          fd_{-1} {}

Wrappers::Pipe::~Pipe() {
    if (fd_ != -1) Close();
}

bool Wrappers::Pipe::Open(Mode mode) {
    if (!FileExists(path_)) {
        mkfifo(path_, 0777);
    }
    fd_ = open(path_, mode);
    return fd_ != -1;
}

bool Wrappers::Pipe::Close() {
    if (unlink(path_) == -1) return false;
    fd_ = -1;
    return true;
}

void Wrappers::Pipe::SetTimeout(int seconds) {
    timeout_ = seconds * 1000U;
}