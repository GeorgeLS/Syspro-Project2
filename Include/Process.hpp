#ifndef EXERCISE_II_PROCESS_HPP
#define EXERCISE_II_PROCESS_HPP

#include <unistd.h>
#include <algorithm>
#include "Report_Utils.hpp"
#include "Array.hpp"
#include "String_Utils.hpp"

using namespace Utils::String;
using namespace Utils::Report;

namespace Wrappers {
    class Process {
    public:
        Process() = delete;

        Process(const char *exe_path, Types::Array<char *> &parameters)
                : argv_{parameters} {
            exe_path_ = AllocateAndCopyString(exe_path);
        }

        ~Process() {
            std::free((void *) exe_path_);
        }

        inline pid_t PID() const { return pid_; }

        void Spawn() {
            pid_t pid = fork();
            if (pid == 0) {
                execv(exe_path_, argv_.C_Array());
            } else if (pid > 0) {
                pid_ = pid;
            } else {
                Die("Couldn't create new process");
            }
        }

    private:
        const char *exe_path_;
        Types::Array<char *> argv_;
        pid_t pid_{};
    };
}


#endif //EXERCISE_II_PROCESS_HPP
