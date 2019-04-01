#include <cinttypes>
#include <iostream>
#include <unistd.h>
#include "../Include/Client_Parameters.hpp"
#include "../Include/Client_Utils.hpp"
#include "../Include/Pipe.hpp"

using namespace Wrappers;
using namespace client;

int main(int argc, char **argv) {
    Client_Parameters arguments = ParseClientParameters(argc, argv);
    char *sender_id_file = argv[argc - 1];
    char *sender_id = GetClientID(sender_id_file);
    std::cout << "Sender ID: " << sender_id << std::endl;
    char *pipe_name;
    asprintf(&pipe_name, "%s/id%" PRIu64 "_to_id%s.fifo",
             arguments.common_dir, arguments.id, sender_id);
    std::cout << pipe_name << std::endl;
    Pipe pipe{pipe_name, arguments.buffer_size};
    pid_t pid = getpid();
    if (pipe.Open(Pipe::Read_Only)) {
        std::cout << "[" << pid << "] Opened pipe " << pipe.Name() << std::endl;
    }
    auto &buffer = pipe.Buffer();
    size_t message_length;

    pipe >> message_length >> buffer;

    std::cout << "Message Length: " << message_length << std::endl;

    std::cout << "Message: " << buffer.C_Array() << std::endl;

//    pipe.Read(sizeof(size_t));
//    size_t message_length = *(size_t *) pipe.Contents();
//    std::cout << "Message Length: " << message_length << std::endl;
//    pipe.Read(message_length);
//    std::cout << "Message: " << pipe.Contents() << std::endl;

    free(pipe_name);
    return (EXIT_SUCCESS);
}