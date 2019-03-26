#ifndef EXERCISE_II_FILE_UTILS_HPP
#define EXERCISE_II_FILE_UTILS_HPP

namespace Utils::File {
    /**
     * FileExists - Checks whether the file/directory of the given path exists;
     * @param path The path to check
     * @return True if the file/directory exists, false otherwise
     */
    bool FileExists(const char *path);

    bool ExistsAndIsDirectory(const char *path);
}

#endif //EXERCISE_II_FILE_UTILS_HPP
