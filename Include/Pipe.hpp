#ifndef EXERCISE_II_PIPE_HPP
#define EXERCISE_II_PIPE_HPP

#include <fcntl.h>
#include "../Include/Array.hpp"

using namespace Types;

namespace Wrappers {
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

        Pipe &operator<<(const char *str);

        Pipe &operator<<(unsigned long v);

        Pipe &operator>>(Array<char> &buffer);

        Pipe &operator>>(unsigned long &v);

        ~Pipe();

        bool Open(Mode mode);

        bool Close();

        void Read(size_t bytes);

        void Write(void *data, size_t bytes);

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
