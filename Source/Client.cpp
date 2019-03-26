#include <cstdlib>
#include <sys/stat.h>
#include <cstdio>
#include <fcntl.h>
#include <unistd.h>
#include <cinttypes>
#include "../Include/Client.hpp"
#include "../Include/String_Utils.hpp"
#include "../Include/Report_Utils.hpp"
#include "../Include/Utils.hpp"
#include "../Include/File_Utils.hpp"

using namespace Utils;
using namespace Utils::String;
using namespace Utils::Report;
using namespace Utils::File;

Client_Parameters Client::ParseClientParameters(int argc, char **argv) {
    int c;
    Client_Parameters arguments{};
    while (true) {
        int option_index;
        c = getopt_long_only(argc, argv, "n:c:i:m:b:l:", options, &option_index);
        if (c == -1) break;
        // NOTE (gliontos): We simply assign the pointer to the argument (for string arguments) and do not copy them
        // because these string are in the argument vector whose memory doesn't move throughout the execution of the program
        switch (c) {
            case 'n': {
                if (!StringToInt64(optarg, (int64_t *) &arguments.id)) {
                    Die("The id provided is invalid!");
                }
                break;
            }
            case 'c' : {
                arguments.common_dir = optarg;
                break;
            }
            case 'i': {
                arguments.input_dir = optarg;
                break;
            }
            case 'm': {
                arguments.mirror_dir = optarg;
                break;
            }
            case 'b': {
                if (!StringToInt64(optarg, (int64_t *) &arguments.buffer_size)) {
                    Die("The buffer_size provided is invalid!");
                }
            }
            case 'l': {
                arguments.log_file = optarg;
                break;
            }
            case 'h':
                Usage();
            case '?':
                break;
            default:
                abort();
        }
    }
    return arguments;
}

void Client::ValidateParameters(Client_Parameters &program_arguments) {
    if (!ExistsAndIsDirectory(program_arguments.input_dir)) {
        Die("The input directory you provided doesn't exist or is not a directory!");
    }
    if (FileExists(program_arguments.mirror_dir)) {
        Die("The mirror directory you provided already exists!");
    } else {
        mkdir(program_arguments.mirror_dir, S_IRWXU);
    }
    if (!FileExists(program_arguments.common_dir)) {
        mkdir(program_arguments.common_dir, S_IRWXU | S_IRWXG | S_IRWXO);
    }
}

void Client::Start() {
    arguments_ = ParseClientParameters(argc_, argv_);
    ValidateParameters(arguments_);
    CreateIDFile();
}

Client::Client(int argc, char **argv)
: argc_{argc}, argv_{argv} {}

void Client::CreateIDFile() {
    char *file_path;
    asprintf(&file_path, "%s/%" PRIu64 ".id", arguments_.common_dir, arguments_.id);
    if (FileExists(file_path)) {
        Die("The ID file for this client already exists!");
    }
    int fd = open(file_path, O_CREAT | O_WRONLY);
    if (fd == -1) {
        Die("Couldn't create ID file for client %" PRIu64 "!", arguments_.id);
    }
    char *pid_str;
    asprintf(&pid_str, "%d", getpid());
    write(fd, pid_str, strlen(pid_str));
    free(pid_str);
    free(file_path);
    close(fd);
}
