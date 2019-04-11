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

typedef struct {
    Pipe *pipe{};
    char *sender_id{};
    char *pipe_name{};
    char *full_path_filename{};
    char *dir_path{};
    int fd{};
} Receiver_Resources;

Receiver_Resources resources;

void Free_Resources() {
    if (resources.pipe) {
        resources.pipe->~Pipe();
    }
    free(resources.sender_id);
    free(resources.pipe_name);
    free(resources.full_path_filename);
    free(resources.dir_path);
    close(resources.fd);
}

// NOTE (gliontos): This is only going to be called from the signals sent by the parent process
void Handle_Signal(int signum) {
    Free_Resources();
    exit(EXIT_FAILURE);
}

void RegisterSignals() {
    struct sigaction action{};
    sigemptyset(&action.sa_mask);
    // Ignore SIGPIPE so we can get -1 if the read end gets closed
    action.sa_handler = SIG_IGN;
    sigaction(SIGPIPE, &action, NULL);
    action.sa_handler = &Handle_Signal;
    sigaction(SIGQUIT, &action, NULL);
}

int main(int argc, char **argv) {
    RegisterSignals();
    Client_Parameters arguments = ParseClientParameters(argc, argv);
	
    auto &sender_id = resources.sender_id;
    auto &pipe_name = resources.pipe_name;
	
    sender_id = GetClientID(argv[argc - 2]);
    asprintf(&pipe_name, "%s/id%s_to_id%" PRIu64 ".fifo",
             arguments.common_dir, sender_id, arguments.id);
	
    Pipe pipe{pipe_name, arguments.buffer_size};
    pipe.SetTimeout(30);
    resources.pipe = &pipe;
	
    if (!pipe.Open(Pipe::Read_Only)) {
        Die("Couldn't open the pipe <%s>. Terminating!", pipe.Name());
    }
	
    int64_t log_fd;
    if (!String::StringToInt64(argv[argc - 1], &log_fd)) {
        Die("Couldn't parse log file descriptor");
    }
	
    auto &dir_path = resources.dir_path;
    asprintf(&dir_path, "%s/%s", arguments.mirror_dir, sender_id);
	
    if (!CreateDirectory(dir_path, S_IRWXU)) {
        Die("Couldn't create directory for client <%s>. Terminating", sender_id);
    }
	
    auto &full_path_filename = resources.full_path_filename;

    while (true) {
        uint16_t filename_length{};
        try {
            pipe >> filename_length;
        } catch (Pipe::PipeException &exception) {
            ReportError("%s", exception.what());
            goto __ERROR__;
        }
		
        if (filename_length == 0U) break;
		
        try {
            pipe.Read(filename_length);
        } catch (Pipe::PipeException &exception) {
            ReportError("%s", exception.what());
            goto __ERROR__;
        }
		
        asprintf(&full_path_filename, "%s/%s", dir_path, pipe.Contents());
		
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
				close(fd);
				remove(full_path_filename);
				goto __ERROR__;
			}
		}
		
		FileReport(log_fd, "[R] %s %" PRIu32 "\n", full_path_filename, original_file_size);
		close(fd);
		free(full_path_filename);
		full_path_filename = NULL;
	}
	
	fsync(log_fd);
    Free_Resources();
	exit(EXIT_SUCCESS);
	
	__ERROR__:
    Free_Resources();
	kill(getppid(), SIGUSR1);
	exit(EXIT_FAILURE);
}