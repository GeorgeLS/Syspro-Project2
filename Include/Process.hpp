#ifndef EXERCISE_II_PROCESS_HPP
#define EXERCISE_II_PROCESS_HPP

#include <unistd.h>
#include "Report_Utils.hpp"
#include "Array.hpp"
#include "String_Utils.hpp"

using namespace Utils::String;
using namespace Utils::Report;

namespace Wrappers {
    class Process {
    public:
        Process() = default;

        Process(const char *exe_path, Types::Array<char *> parameters)
        : argv_{std::move(parameters)} {
            exe_path_ = AllocateAndCopyString(exe_path);
        }

        ~Process() {
            std::free((void *) exe_path_);
        }

        void Spawn() {
            pid_t pid = fork();
            if (pid > 0) {
                execv(exe_path_, argv_.C_Array());
            } else if (pid < 0) {
                Die("Couldn't create new process");
            }
        }

    private:
        const char *exe_path_;
        Types::Array<char *> argv_;
    };
}


#endif //EXERCISE_II_PROCESS_HPP
