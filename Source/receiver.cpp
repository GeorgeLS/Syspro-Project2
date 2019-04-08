#include <cinttypes>
#include <iostream>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <signal.h>
#include "../Include/Client_Parameters.hpp"
#include "../Include/Client_Utils.hpp"
#include "../Include/Pipe.hpp"
#include "../Include/String_Utils.hpp"
#include "../Include/File_Utils.hpp"
#include "../Include/Utils.hpp"

using namespace Wrappers;
using namespace String;
using namespace File;
using namespace client;

int main(int argc, char **argv) {
    Client_Parameters arguments = ParseClientParameters(argc, argv);
    char *sender_id = GetClientID(argv[argc - 2]);
    char *pipe_name;
    asprintf(&pipe_name, "%s/id%s_to_id%" PRIu64 ".fifo",
             arguments.common_dir, sender_id, arguments.id);

    Pipe pipe{pipe_name, arguments.buffer_size};
    free(pipe_name);

    if (!pipe.Open(Pipe::Read_Only)) {
        Die("Couldn't open the pipe <%s>. Terminating!", pipe.Name());
    }

    int64_t log_fd;
    if (!String::StringToInt64(argv[argc - 1], &log_fd)) {
        Die("Couldn't parse log file descriptor");
    }

    char *dir_path;
    asprintf(&dir_path, "%s/%s", arguments.mirror_dir, sender_id);

    if (!CreateDirectory(dir_path, S_IRWXU)) {
        Die("Couldn't create directory for client <%s>. Terminating", sender_id);
    }

    while (true) {
        uint16_t filename_length{};
        pipe.Read(sizeof(uint16_t));
        filename_length = pipe.GetContentsAs<uint16_t>();
        if (filename_length == 0U) break;

        pipe.Read(filename_length);
        char *filename = AllocateAndCopyString(pipe.Contents());
        char *full_path_filename;
        asprintf(&full_path_filename, "%s/%s", dir_path, filename);

        uint32_t file_size{};
        pipe >> file_size;
        uint32_t original_file_size = file_size;

        // This will create the directory structure of the file to receive if any
        if (!CreateDirectory(full_path_filename)) {
            Die("Something went wrong while trying to create the directory structure for the file to receive");
        }

        int fd = open(full_path_filename, O_CREAT | O_WRONLY, ACCESSPERMS);
        if (fd == -1) {
            Die("Couldn't open file <%s>", full_path_filename);
        }

        while (file_size > 0) {
            ssize_t bytes_read = pipe.Read(file_size);
            char *contents = pipe.Contents();
            contents[bytes_read] = '\0';
            write(fd, contents, bytes_read);
            file_size -= bytes_read;
        }

        flock(log_fd, LOCK_EX);
        FileReport(log_fd, "%s %" PRIu32 "", filename, original_file_size);
        flock(log_fd, LOCK_UN);

        close(fd);
        free(filename);
        free(full_path_filename);
    }

    free(dir_path);
    fsync(log_fd);
    close(log_fd);
    return EXIT_SUCCESS;
}