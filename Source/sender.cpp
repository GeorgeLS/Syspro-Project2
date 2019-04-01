#include <cstdio>
#include <cinttypes>
#include <iostream>
#include <unistd.h>
#include "../Include/Pipe.hpp"
#include "../Include/Client_Parameters.hpp"
#include "../Include/Client_Utils.hpp"
#include "../Include/String_Utils.hpp"

using namespace client;
using namespace Wrappers;
using namespace String;

int main(int argc, char **argv) {
    Client_Parameters arguments = ParseClientParameters(argc, argv);
    char *receiver_id_file = argv[argc - 1];
    char *receiver_id = GetClientID(receiver_id_file);
    std::cout << "Receiver ID: " << receiver_id << std::endl;
    char *pipe_name;
    asprintf(&pipe_name, "%s/id%" PRIu64 "_to_id%s.fifo",
             arguments.common_dir, arguments.id, receiver_id);
    std::cout << pipe_name << std::endl;
    pid_t pid = getpid();

    Pipe pipe{pipe_name, arguments.buffer_size};
    if (pipe.Open(Pipe::Write_Only)) {
        std::cout << "[" << pid << "] Opened pipe " << pipe.Name() << std::endl;
    }

    char *data = const_cast<char *>("Kalispera");
    size_t data_length = strlen(data) + 1;
    std::cout << "Writing to pipe: " << std::endl;
    pipe << data_length << data;

//    pipe.Write(&data_length, sizeof(size_t));
//    pipe.Write(data, data_length);

    return EXIT_SUCCESS;
}