#include <sys/stat.h>
#include "../Include/File_Utils.hpp"

bool Utils::File::FileExists(const char *path) {
    struct stat info{};
    return stat(path, &info) == 0;
}

bool Utils::File::ExistsAndIsDirectory(const char *path) {
    struct stat info{};
    int res = stat(path, &info);
    return res == 0 && ((info.st_mode & S_IFDIR) != 0);
}