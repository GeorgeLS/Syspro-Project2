#ifndef EXERCISE_II_FILE_UTILS_HPP
#define EXERCISE_II_FILE_UTILS_HPP

#include <cstddef>
#include <sys/stat.h>

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

    /**
     * GetFileSize - Gets the size of the specified file
     * @param filename The file to get it's size
     * @return On success returns the size of the file,
     * On failure returns -1
     */
    ssize_t GetFileSize(const char *filename);

    /**
     * PathIncludesDirectory - Checks whether the given path has a directory in it
     * @param path The path to check
     * @return On success returns true,
     * On failure returns false.
     */
    bool PathIncludesDirectory(const char *path);

    /**
     * CreateDirectory - Creates the directory of the given path. It creates the whole
     * structure if the path includes multiple directories
     * @param path The path to create the directory(ies) from
     * @param permissions The permissions to give to the directory(ies) to be created. The default is ALLPERMS
     * @returns Returns true if the directory(ies) created successfully,
     * else returns false
     * NOTE (gliontos): You must always include the / at the end of the string.
     * i.e george/ , george/john/
     */
    bool CreateDirectory(char *path, mode_t permissions = ACCESSPERMS);

    /**
     * DeleteDirectory - Deletes the directory specified by the given path. If the directory is not empty then
     * it deletes everything recursively
     * @param path The path to the directory to delete
     */
    void DeleteDirectory(const char *path);
}

#endif //EXERCISE_II_FILE_UTILS_HPP
