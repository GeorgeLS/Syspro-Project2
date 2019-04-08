#ifndef EXERCISE_II_CLIENT_UTILS_HPP
#define EXERCISE_II_CLIENT_UTILS_HPP

#include <csignal>
#include "Client_Parameters.hpp"
#include "../Include/Array.hpp"
#include "List.hpp"

using namespace Types;

namespace Utils::client {
    /**
     * ParseClientParameters - Parses the command line arguments given to the client
     * @param argc The number of command line arguments (including the program name)
     * @param argv The argument strings (argument vector; including the program name)
     * @return On success returns a struct that represents the clients arguments,
     * terminates on failure
     */
    Client_Parameters ParseClientParameters(int argc, char **argv);

    /**
     * ValidateParameters - Validates the parameters based on the client requirements
     * @param parameters The struct of client parameters to validate
     */
    void ValidateParameters(Client_Parameters &parameters);

    /**
     * CreateIDFile - Creates a file in the common directory provided by the client parameters
     * with the format <client_id>.id
     * @param parameters The client parameters
     */
    void CreateIDFile(Client_Parameters &parameters);

    /**
     * CreateLogFile - Creates the log file for this client
     * @param filename The log filename
     */
    void CreateLogFile(const char *filename);

    /**
     * GetRegularFiles - Returns an array of strings of all the regular files in the directory
     * @param path The path to search for the files
     * @param full_path If full_path is specified as true then the path provided will be
     * appended to the filename. Default value is false
     * @return On success returns an array of the regular files,
     * On failure returns an empty array
     */
    Array<char *> GetRegularFiles(const char *path, bool full_path = false);

    void GetRegularFilesRecursively(const char *path, List<char *> &result);

    /**
     * GetClientID - Parses the client_filename and returns a newly allocated string containing only the id of the client
     * @param client_filename The client filename to parse
     * @return A newly allocated string containing the id of the client
     */
    char *GetClientID(const char *client_filename);

    /**
     * HandleSignal - When a signal is received it calls the proper function of the client (global_client)
     * @param signum The signal number received
     * @param info The signal information
     * @param context A pointer to ucontext_t structure passed as void *. We don't use it
     */
    void HandleSignal(int signum, siginfo_t *info, void *context);
}

#endif //EXERCISE_II_CLIENT_UTILS_HPP
