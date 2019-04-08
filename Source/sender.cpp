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

// TODO(gliontos): Register signals in order for the parent process
// to kindly request this child to terminate (free all it's memory, close any open
// file descriptors etc...
int main(int argc, char **argv) {
    Client_Parameters arguments = ParseClientParameters(argc, argv);
    char *receiver_id = GetClientID(argv[argc - 2]);
    char *pipe_name;
    asprintf(&pipe_name, "%s/id%" PRIu64 "_to_id%s.fifo",
             arguments.common_dir, arguments.id, receiver_id);
	
    Pipe pipe{pipe_name, arguments.buffer_size};
    free(pipe_name);
	
    if (!pipe.Open(Pipe::Write_Only)) {
        Die("Couldn't open the pipe <%s>. Terminating!", pipe.Name());
    }
	
    List<char *> input_files{};
    GetRegularFilesRecursively(arguments.input_dir, input_files);
	
    Array<char> file_buffer(arguments.buffer_size);
	
    size_t input_dir_length = strlen(arguments.input_dir) + 1;
	
    // Ignore SIGPIPE so we can get -1 if the read end gets closed
    struct sigaction action{};
    sigemptyset(&action.sa_mask);
    action.sa_handler = SIG_IGN;
    sigaction(SIGPIPE, &action, NULL);
	
    FOREACH (input_files) {
        char *full_path_filename = *it;
        // Get the length of the filename without the length of the input directory
        uint16_t filename_length = strlen(full_path_filename) - input_dir_length;
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
		
        try {
            pipe << filename_length << (full_path_filename + input_dir_length) << file_size;
        } catch (Pipe::PipeException &exception) {
			close(fd);
			goto __ERROR__;
        }
		
        size_t bytes_read;
        while ((bytes_read = read(fd, file_buffer.C_Array(), file_buffer.Size())) != 0) {
			try {
				char *contents = file_buffer.C_Array();
				contents[bytes_read] = '\0';
				pipe << contents;
			} catch (Pipe::PipeException &exception) {
				close(fd);
				goto __ERROR__;
			}
		}
		
		close(fd);
		__LOOP_END__:
		free(full_path_filename);
	}
	
	try {
		pipe << uint16_t{0};
	} catch (Pipe::PipeException &exception) {
		goto __ERROR__;
	}
	return EXIT_SUCCESS;
	
	__ERROR__:
	kill(getppid(), SIGUSR1);
	return EXIT_FAILURE;
}