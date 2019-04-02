#include <cinttypes>
#include <iostream>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/file.h>
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
    asprintf(&pipe_name, "%s/id%s_to_id%" PRIu64 ".fifo",
             arguments.common_dir, sender_id, arguments.id);
    Pipe pipe{pipe_name, arguments.buffer_size};
    free(pipe_name);
    if (!pipe.Open(Pipe::Read_Only)) {
        Die("Couldn't open the pipe <%s>. Terminating!", pipe.Name());
    }
    int log_fd = open(arguments.log_file, O_APPEND);
    if (log_fd == -1) {
        Die("Couldn't open log file <%s>. Terminating!", arguments.log_file);
    }
    char *dir_path;
    asprintf(&dir_path, "%s/%s", arguments.mirror_dir, sender_id);
    if (mkdir(dir_path, S_IRWXU) == -1) {
        Die("Couldn't create directory for client <%s>. Terminating", sender_id);
    }
    struct flock file_lock{};
    memset(&file_lock, '\0', sizeof(struct flock));
    file_lock.l_whence = SEEK_SET;
    file_lock.l_start = 0;
    file_lock.l_len = 0;

    while (true) {
        uint16_t filename_length{};
        pipe >> filename_length;
        if (filename_length == 0U) break;
        pipe.Read(filename_length);
        char *filename = AllocateAndCopyString(pipe.Contents());
        char *full_path_filename;
        asprintf(&full_path_filename, "%s/%s", dir_path, filename);
        uint32_t file_size{};
        pipe >> file_size;
        uint32_t original_file_size = file_size;
        int fd = open(full_path_filename, O_CREAT | O_WRONLY, 0777);
        if (fd == -1) {
            Die("Couldn't create file <%s>", full_path_filename);
        }
        while (file_size > 0) {
            ssize_t bytes_read = pipe.Read(file_size);
            char *contents = pipe.Contents();
            contents[bytes_read] = '\0';
            write(fd, contents, bytes_read);
            file_size -= bytes_read;
        }
        file_lock.l_type = F_WRLCK;
        fcntl(log_fd, F_SETLK, &file_lock);
        FileReport(log_fd, "Received file <%s>. Number of bytes: %" PRIu32 "", full_path_filename, original_file_size);
        file_lock.l_type = F_UNLCK;
        fcntl(log_fd, F_SETLK, &file_lock);
        close(fd);
        free(filename);
        free(full_path_filename);
    }
    free(dir_path);
    close(log_fd);
    return (EXIT_SUCCESS);
}