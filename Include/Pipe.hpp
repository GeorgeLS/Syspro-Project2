#ifndef EXERCISE_II_PIPE_HPP
#define EXERCISE_II_PIPE_HPP

#include <poll.h>
#include <fcntl.h>
#include "../Include/Array.hpp"

using namespace Types;

namespace Wrappers {
    class Pipe {
    public:
        enum Mode {
            Read_Only = O_RDONLY,
            Write_Only = O_WRONLY,
            Read_Write = O_RDWR,
            Non_Blocking = O_NONBLOCK
        };

        Pipe() = delete;

        Pipe(const char *path, size_t buffer_size);

        Pipe(const Pipe &other) = delete;

        Pipe(Pipe &&other) = delete;

        Pipe &operator=(const Pipe &other) = delete;

        Pipe &operator=(Pipe &&other) = delete;

        template<typename T>
        Pipe &Write(const T &data) {
            write(fd_, &data, sizeof(T));
            return *this;
        }

        Pipe &Write(void *data, size_t bytes) {
            write(fd_, data, bytes);
            return *this;
        }

        ssize_t Read(size_t bytes) {
            memset(buffer_.C_Array(), '\0', buffer_.Size());
            if (bytes > buffer_.Size()) {
                bytes = buffer_.Size();
            }
            if (timeout_ != -1) {
                struct pollfd pfd = {0};
                pfd.fd = fd_;
                pfd.events = POLLIN;
                if (poll(&pfd, 1, timeout_) == 0) return -1;
            }
            return read(fd_, buffer_.C_Array(), bytes);
        }

        Pipe &operator<<(const char *str) {
            return Write((void *) str, strlen(str));
        }

        template<typename T, typename std::enable_if_t<std::is_integral_v<T>, bool> = true>
        Pipe &operator<<(const T &value) { return Write(value); }

        template<typename T, typename std::enable_if_t<std::is_integral_v<T>, bool> = true>
        Pipe &operator>>(const T &value) {
            read(fd_, (void *) &value, sizeof(T));
            return *this;
        }

        Pipe &operator>>(Array<char> &buffer) {
            read(fd_, buffer.C_Array(), buffer.Size());
            return *this;
        }

        ~Pipe();

        bool Open(Mode mode);

        bool Close();

        void SetTimeout(int seconds);

        inline Array<char> &Buffer() { return buffer_; }

        inline size_t BufferSize() const { return buffer_.Size(); }

        inline char *Contents() const { return buffer_.C_Array(); };

        template<typename T>
        inline T GetContentsAs() { return *(T *) buffer_.C_Array(); };

        inline const char *Name() const { return path_; };

        bool error{};
    private:
        Array<char> buffer_;
        const char *path_;
        int fd_;
        int timeout_{-1};
    };
}


#endif //EXERCISE_II_PIPE_HPP
