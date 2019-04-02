#include <cinttypes>
#include <iostream>
#include <unistd.h>
#include "../Include/Client_Parameters.hpp"
#include "../Include/Client_Utils.hpp"
#include "../Include/Pipe.hpp"
#include "../Include/String_Utils.hpp"

using namespace Wrappers;
using namespace String;
using namespace client;

int main(int argc, char **argv) {
    Client_Parameters arguments = ParseClientParameters(argc, argv);
    char *sender_id_file = argv[argc - 1];
    char *sender_id = GetClientID(sender_id_file);
    char *pipe_name;
    asprintf(&pipe_name, "%s/id%" PRIu64 "_to_id%s.fifo",
             arguments.common_dir, arguments.id, sender_id);
    Pipe pipe{pipe_name, arguments.buffer_size};
    free(pipe_name);
    if (!pipe.Open(Pipe::Read_Only)) {
        Die("Couldn't open the pipe <%s>. Terminating!", pipe.Name());
    }
    while (true) {
        uint16_t filename_length{};
        pipe >> filename_length;
        if (filename_length == 0U) break;
        pipe.Read(filename_length);
        char *filename = AllocateAndCopyString(pipe.Contents());
        char *full_path_filename;
        asprintf(&full_path_filename, "%s/%s", arguments.mirror_dir, filename);
        free(filename);
        uint32_t file_size{};
        pipe >> file_size;
        int fd = open(full_path_filename, O_CREAT | O_WRONLY, S_IRWXU);
        if (fd == -1) {
            ReportError("Couldn't create file <%s>", full_path_filename);
            break;
        }
        ssize_t bytes_read;
        while ((bytes_read = pipe.Read(arguments.buffer_size)) != 0) {
            char *contents = pipe.Contents();
            contents[bytes_read] = '\0';
            std::cout << "GOT: " << contents << std::endl;
            write(fd, contents, bytes_read);
        }
        close(fd);
        free(full_path_filename);
    }
    return (EXIT_SUCCESS);
}