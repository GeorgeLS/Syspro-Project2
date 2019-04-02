#ifndef EXERCISE_II_PIPE_HPP
#define EXERCISE_II_PIPE_HPP

#include <fcntl.h>
#include "../Include/Array.hpp"

using namespace Types;

namespace Wrappers {
    // TODO (gliontos): Extend this so we can set timeouts
    class Pipe {
    public:
        enum Mode {
            Read_Only = O_RDONLY,
            Write_Only = O_WRONLY,
            Read_Write = O_RDWR
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

        inline Array<char> &Buffer() { return buffer_; }

        inline size_t BufferSize() const { return buffer_.Size(); }

        inline char *Contents() const { return buffer_.C_Array(); };

        inline const char *Name() const { return path_; };

    private:
        Array<char> buffer_;
        const char *path_;
        int fd_;
    };
}


#endif //EXERCISE_II_PIPE_HPP
