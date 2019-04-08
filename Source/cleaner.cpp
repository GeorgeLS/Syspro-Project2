#include <cstdio>
#include <cstdlib>
#include "../Include/File_Utils.hpp"
#include "../Include/Client_Utils.hpp"

using namespace Utils::File;
using namespace client;

int main(int argc, char **argv) {
    Client_Parameters arguments = ParseClientParameters(argc, argv);
    char *client_id = GetClientID(argv[argc - 2]);
    char *path;
    asprintf(&path, "%s/%s", arguments.mirror_dir, client_id);
    DeleteDirectory(path);
    free(path);
    free(client_id);
    return EXIT_SUCCESS;
}