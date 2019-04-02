#include <cinttypes>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <cstdio>
#include <dirent.h>
#include "../Include/Client_Utils.hpp"
#include "../Include/String_Utils.hpp"
#include "../Include/File_Utils.hpp"
#include "../Include/Utils.hpp"

using namespace String;
using namespace File;

Client_Parameters client::ParseClientParameters(int argc, char **argv) {
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

void client::ValidateParameters(Client_Parameters &parameters) {
    if (!ExistsAndIsDirectory(parameters.input_dir)) {
        Die("The input directory you provided doesn't exist or is not a directory!");
    }
    if (FileExists(parameters.mirror_dir)) {
        Die("The mirror directory you provided already exists!");
    } else {
        mkdir(parameters.mirror_dir, S_IRWXU);
    }
    if (!FileExists(parameters.common_dir)) {
        mkdir(parameters.common_dir, S_IRWXU | S_IRWXG | S_IRWXO);
    }
}

void client::CreateIDFile(Client_Parameters &parameters) {
    char *file_path;
    asprintf(&file_path, "%s/%" PRIu64 ".id", parameters.common_dir, parameters.id);
    if (FileExists(file_path)) {
        Die("The ID file for this client already exists!");
    }
    int fd = open(file_path, O_CREAT | O_WRONLY, 0777);
    if (fd == -1) {
        Die("Couldn't create ID file for client %" PRIu64 "!", parameters.id);
    }
    int pid = getpid();
    write(fd, &pid, sizeof(int));
    free(file_path);
    close(fd);
}

Array<char *> client::GetRegularFiles(const char *path) {
    ssize_t file_n = Utils::File::CountReguralFilesInDirectory(path);
    if (file_n == -1) {
        return Array<char *>{};
    }
    DIR *dirp = opendir(path);
    struct dirent *entry;
    Array<char *> files{static_cast<size_t>(file_n)};
    size_t i = 0U;
    while ((entry = readdir(dirp)) != NULL) {
        if (entry->d_type == DT_REG) {
            files[i++] = AllocateAndCopyString(entry->d_name);
        }
    }
    closedir(dirp);
    return files;
}

char *client::GetClientID(const char *client_filename) {
    char *id = AllocateAndCopyString(client_filename);
    size_t i = strlen(id);
    while (id[i] != '.') --i;
    id[i] = '\0';
    return id;
}

void client::CreateLogFile(const char *filename) {
    int fd = creat(filename, 0777);
    if (fd == -1) {
        Die("Couldn't create log file <%s>. Terminating!", filename);
    }
    close(fd);
}
