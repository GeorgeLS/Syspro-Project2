#include <cstdio>
#include <cinttypes>
#include <iostream>
#include <unistd.h>
#include <signal.h>
#include "../Include/Pipe.hpp"
#include "../Include/Client_Parameters.hpp"
#include "../Include/Client_Utils.hpp"
#include "../Include/String_Utils.hpp"
#include "../Include/File_Utils.hpp"
#include "../Include/Utils.hpp"

using namespace client;
using namespace Wrappers;
using namespace String;
using namespace File;

typedef struct {
    List<char *> input_files{};
    Array<char> file_buffer{};
    Pipe *pipe{};
    char *receiver_id{};
    char *pipe_name{};
    int fd{};
} Receiver_Resources;

Receiver_Resources resources;

void Free_Resources() {
    FOREACH (resources.input_files) {
        free(*it);
    }
    resources.input_files.~List();
    if (resources.pipe) {
        resources.pipe->~Pipe();
    }
    free(resources.receiver_id);
    free(resources.pipe_name);
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
	
	auto &receiver_id = resources.receiver_id;
	auto &pipe_name = resources.pipe_name;
	
	receiver_id = GetClientID(argv[argc - 2]);
	asprintf(&pipe_name, "%s/id%" PRIu64 "_to_id%s.fifo",
			 arguments.common_dir, arguments.id, receiver_id);
	
    int64_t log_fd;
    if (!String::StringToInt64(argv[argc - 1], &log_fd)) {
        Die("Couldn't parse log file descriptor");
    }
	
	Pipe pipe{pipe_name, arguments.buffer_size};
	resources.pipe = &pipe;
	
	if (!pipe.Open(Pipe::Write_Only)) {
		Die("Couldn't open the pipe <%s>. Terminating!", pipe.Name());
	}
	
	auto &input_files = resources.input_files;
	auto &file_buffer = resources.file_buffer;
	
	GetRegularFilesRecursively(arguments.input_dir, input_files);
	resources.file_buffer.Reserve(arguments.buffer_size);
	
	size_t input_dir_length = strlen(arguments.input_dir) + 1;
	auto &fd = resources.fd;
	
	FOREACH (input_files) {
		char *full_path_filename = *it;
		
		// Get the length of the filename without the length of the input directory
		uint16_t filename_length = strlen(full_path_filename) - input_dir_length;
		int32_t file_size = GetFileSize(full_path_filename);
		if (file_size == -1) {
			ReportError("Couldn't get the file size of file <%s>", full_path_filename);
            continue;
		}
		
		fd = open(full_path_filename, O_RDONLY);
		if (fd == -1) {
			ReportError("Couldn't open the file <%s>", full_path_filename);
            continue;
		}
		
		try {
			pipe << filename_length << (full_path_filename + input_dir_length) << file_size;
		} catch (Pipe::PipeException &exception) {
			ReportError("%s", exception.what());
			goto __ERROR__;
		}
		
		ssize_t bytes_read;
		while ((bytes_read = read(fd, file_buffer.C_Array(), file_buffer.Size() - 1)) != 0) {
			if (bytes_read == -1) {
				ReportError("There was an error while reading the file <%s>", full_path_filename);
				goto __ERROR__;
			}
			
			try {
				char *contents = file_buffer.C_Array();
				contents[bytes_read] = '\0';
				pipe << contents;
			} catch (Pipe::PipeException &exception) {
				ReportError("%s", exception.what());
				goto __ERROR__;
			}
		}
		
		FileReport(log_fd, "[S] %s %" PRIu32 "\n", full_path_filename + input_dir_length, file_size);
		close(fd);
	}
	
	try {
		pipe << uint16_t{0};
	} catch (Pipe::PipeException &exception) {
		ReportError("%s", exception.what());
		goto __ERROR__;
	}
	
	Free_Resources();
	exit(EXIT_SUCCESS);
	
	__ERROR__:
	Free_Resources();
	kill(getppid(), SIGUSR1);
	exit(EXIT_FAILURE);
}