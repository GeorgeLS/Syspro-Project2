#include <iostream>
#include "../Include/Utils.hpp"

void Utils::Usage() {
    fprintf(stderr,
            "You must provide the following arguments:\n"
            "\t-n <id>\n"
            "\t-c <common_directory>\n"
            "\t-i <input_directory>\n"
            "\t-m <mirror_directory>\n"
            "\t-b <pipe_buffer_size>\n"
            "\t-l <log_file>\n");
    exit(EXIT_FAILURE);
}