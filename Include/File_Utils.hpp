#ifndef EXERCISE_II_FILE_UTILS_HPP
#define EXERCISE_II_FILE_UTILS_HPP

#include <cstddef>

namespace Utils::File {
    /**
     * FileExists - Checks whether the file/directory of the given path exists;
     * @param path The path to check
     * @return True if the file/directory exists, false otherwise
     */
    bool FileExists(const char *path);

    /**
     * ExistAndIsDirectory - Checks whether the the given path exists and is a directory
     * @param path The path to check
     * @return True if the path exists and is a directory, false otherwise
     */
    bool ExistsAndIsDirectory(const char *path);

    /**
     * CountRegularFilesInDirectory - Counts the number of regular files in the given path (directory)
     * @param path The directory path to count it's files
     * @return The number of regular files in the directory on success, -1 on false
     */
    ssize_t CountReguralFilesInDirectory(const char *path);
}

#endif //EXERCISE_II_FILE_UTILS_HPP
