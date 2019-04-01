#ifndef EXERCISE_II_PROGRAM_ARGUMENTS_HPP
#define EXERCISE_II_PROGRAM_ARGUMENTS_HPP

#include <cstdint>
#include <cstddef>
#include <getopt.h>

static struct option options[] = {
        {"n", required_argument, NULL, 'n'},
        {"c", required_argument, NULL, 'c'},
        {"i", required_argument, NULL, 'i'},
        {"m", required_argument, NULL, 'm'},
        {"b", required_argument, NULL, 'b'},
        {"l", required_argument, NULL, 'l'},
        {"h", no_argument,       NULL, 'h'},
        {0, 0, 0,                      0}
};

struct Client_Parameters {
    Client_Parameters() = default;

    Client_Parameters(const Client_Parameters &arguments) = default;

    Client_Parameters(Client_Parameters &&arguments) = default;

    Client_Parameters &operator=(const Client_Parameters &arguments) = default;

    Client_Parameters &operator=(Client_Parameters &&arguments) = default;

    ~Client_Parameters() = default;

    uint64_t id{};
    const char *common_dir{};
    const char *input_dir{};
    const char *mirror_dir{};
    size_t buffer_size{};
    const char *log_file{};
    size_t arguments_n{12U};
};

#endif //EXERCISE_II_PROGRAM_ARGUMENTS_HPP
