#include <sys/stat.h>
#include <dirent.h>
#include <cstdio>
#include "../Include/File_Utils.hpp"
#include "../Include/Report_Utils.hpp"

using namespace Utils::Report;

bool Utils::File::FileExists(const char *path) {
    struct stat info{};
    return stat(path, &info) == 0;
}

bool Utils::File::ExistsAndIsDirectory(const char *path) {
    struct stat info{};
    int res = stat(path, &info);
    return res == 0 && ((info.st_mode & S_IFDIR) != 0);
}

ssize_t Utils::File::CountReguralFilesInDirectory(const char *path) {
    ssize_t count = 0;
    DIR *dirp;
    struct dirent *entry;
    dirp = opendir(path);
    if (dirp == NULL) {
        ReportError("There is no such directory <%s>", path);
        return -1;
    }
    while ((entry = readdir(dirp)) != NULL) {
        if (entry->d_type == DT_REG) ++count;
    }
    closedir(dirp);
    return count;
}

ssize_t Utils::File::GetFileSize(const char *filename) {
    struct stat info{};
    if (stat(filename, &info) == -1) return -1;
    return info.st_size;
}
