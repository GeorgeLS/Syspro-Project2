#ifndef EXERCISE_II_PIPE_HPP
#define EXERCISE_II_PIPE_HPP

#include <poll.h>
#include <fcntl.h>
#include <type_traits>
#include "../Include/Array.hpp"

using namespace Types;

namespace Wrappers {
    class Pipe {
    public:
        class PipeException : public std::exception {
        public:
            PipeException(const char *message) : message_{message} {};

            const char *what() const throw() override {
                return message_;
            }

        private:
            const char *message_;
        };

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
            if (write(fd_, &data, sizeof(T)) == -1) {
                throw PipeException("Error while writing to pipe");
            }
            return *this;
        }

        Pipe &Write(void *data, size_t bytes) {
            if (write(fd_, data, bytes) == -1) {
                throw PipeException("Error while writing to pipe");
            }
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
                if (poll(&pfd, 1, timeout_) == 0) {
                    throw PipeException("Timeout limit exceeded");
                }
            }
            ssize_t res = read(fd_, buffer_.C_Array(), bytes);
            if (res == -1) {
                throw PipeException("Error while reading");
            }
            return res;
        }

        Pipe &operator<<(const char *str) {
            return Write((void *) str, strlen(str));
        }

        template<typename T, typename std::enable_if<std::is_integral<T>::value, bool>::type = true>
        Pipe &operator<<(const T &value) { return Write(value); }

        template<typename T, typename std::enable_if<std::is_integral<T>::value, bool>::type = true>
        Pipe &operator>>(T &value) {
            if (Read(sizeof(T)) == 0) {
                value = 0;
            } else {
                value = GetContentsAs<T>();
            }
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
