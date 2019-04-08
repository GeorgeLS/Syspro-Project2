#include <sys/stat.h>
#include <dirent.h>
#include <cstdio>
#include <malloc.h>
#include <cerrno>
#include "../Include/File_Utils.hpp"
#include "../Include/Report_Utils.hpp"
#include "../Include/String_Utils.hpp"

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

bool Utils::File::PathIncludesDirectory(const char *path) {
    while (*path) {
        if (*path == '/') return true;
        ++path;
    }
    return false;
}

bool Utils::File::CreateDirectory(char *path, mode_t permissions) {
    bool result{true};
    char *copy = Utils::String::AllocateAndCopyString(path);
    char *start = copy;
    while (*copy) {
        if (*copy == '/') {
            *copy = '\0';
            errno = 0;
            int res = mkdir(start, permissions);
            if (res == -1) {
                if (errno == EEXIST) {
                    // In this case we actually care if it's a directory.
                    // If errno is EEXIST we know for a fact that the file/directory exists
                    // So if the path points to a regular file that already exists we return false
                    if (!ExistsAndIsDirectory(start)) {
                        result = false;
                        goto __EXIT__;
                    }
                } else {
                    result = false;
                    goto __EXIT__;
                }
            }
            *copy = '/';
        }
        ++copy;
    }
    __EXIT__:
    free(start);
    return result;
}
