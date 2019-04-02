#include <cstdio>
#include <cinttypes>
#include <iostream>
#include <unistd.h>
#include "../Include/Pipe.hpp"
#include "../Include/Client_Parameters.hpp"
#include "../Include/Client_Utils.hpp"
#include "../Include/String_Utils.hpp"
#include "../Include/File_Utils.hpp"

using namespace client;
using namespace Wrappers;
using namespace String;
using namespace File;

int main(int argc, char **argv) {
    Client_Parameters arguments = ParseClientParameters(argc, argv);
    char *receiver_id_file = argv[argc - 1];
    char *receiver_id = GetClientID(receiver_id_file);
    char *pipe_name;
    asprintf(&pipe_name, "%s/id%" PRIu64 "_to_id%s.fifo",
             arguments.common_dir, arguments.id, receiver_id);

    Pipe pipe{pipe_name, arguments.buffer_size};
    free(pipe_name);

    if (!pipe.Open(Pipe::Write_Only)) {
        Die("Couldn't open the pipe <%s>. Terminating!", pipe.Name());
    }

    Array<char *> input_files = GetRegularFiles(arguments.input_dir);
    Array<char> file_buffer{static_cast<char>(arguments.buffer_size)};

    for (size_t i = 0U; i != input_files.Size(); ++i) {
        char *filename = input_files[i];
        uint16_t filename_length = strlen(filename);
        char *full_path_filename;
        asprintf(&full_path_filename, "%s/%s", arguments.input_dir, filename);
        uint32_t file_size = GetFileSize(full_path_filename);
        if (file_size == -1) {
            ReportError("Couldn't get the file size of file <%s>", full_path_filename);
            continue;
        }
        int fd = open(full_path_filename, O_RDONLY);
        if (fd == -1) {
            ReportError("Couldn't open the file <%s>", full_path_filename);
            goto __LOOP_END__;
        }
        pipe << filename_length << filename << file_size;
        size_t bytes_read;
        while ((bytes_read = read(fd, file_buffer.C_Array(), file_buffer.Size())) != 0) {
            char *contents = file_buffer.C_Array();
            contents[bytes_read] = '\0';
            pipe << contents;
        }
        close(fd);
        __LOOP_END__:
        free(full_path_filename);
    }
    pipe << uint16_t{0};
    return EXIT_SUCCESS;
}