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

// TODO(gliontos): Register signals in order for the parent process
// to kindly request this child to terminate (free all it's memory, close any open
// file descriptors etc...
int main(int argc, char **argv) {
    Client_Parameters arguments = ParseClientParameters(argc, argv);
    char *sender_id = GetClientID(argv[argc - 2]);
    char *pipe_name;
    asprintf(&pipe_name, "%s/id%s_to_id%" PRIu64 ".fifo",
             arguments.common_dir, sender_id, arguments.id);
	
    Pipe pipe{pipe_name, arguments.buffer_size};
    pipe.SetTimeout(30);
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
        try {
            pipe >> filename_length;
        } catch (Pipe::PipeException &exception) {
            goto __ERROR__;
        }
        if (filename_length == 0U) break;
		
        try {
            pipe.Read(filename_length);
        } catch (Pipe::PipeException &exception) {
            goto __ERROR__;
        }
        char *filename = AllocateAndCopyString(pipe.Contents());
        char *full_path_filename;
        asprintf(&full_path_filename, "%s/%s", dir_path, filename);
		
        uint32_t file_size{};
        try {
            pipe >> file_size;
        } catch (Pipe::PipeException &exception) {
            goto __ERROR__;
        }
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
            try {
                ssize_t bytes_read = pipe.Read(file_size);
                char *contents = pipe.Contents();
                contents[bytes_read] = '\0';
                write(fd, contents, bytes_read);
                file_size -= bytes_read;
            } catch (Pipe::PipeException &exception) {
				// TODO(gliontos): Add buffering here se won't write on the file directly
				close(fd);
				remove(full_path_filename);
				goto __ERROR__;
			}
		}
		FileReport(log_fd, "%s %" PRIu32 "\n", filename, original_file_size);
		close(fd);
		free(filename);
		free(full_path_filename);
	}
	
	fsync(log_fd);
	free(dir_path);
	return EXIT_SUCCESS;
	
	__ERROR__:
	free(dir_path);
	kill(getppid(), SIGUSR1);
	return EXIT_FAILURE;
}