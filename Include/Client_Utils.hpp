#ifndef EXERCISE_II_CLIENT_UTILS_HPP
#define EXERCISE_II_CLIENT_UTILS_HPP

#include "Client_Parameters.hpp"
#include "../Include/Array.hpp"

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
     * GetClients - Returns an array of strings of all the client filenames in the common directory
     * @param path The path to search for the client files
     * @return On success returns an array of the client filenames,
     * On failure returns an empty array
     */
    Array<char *> GetClients(const char *path);

    /**
     * GetClientID - Parses the client_filename and returns a newly allocated string containing only the id of the client
     * @param client_filename The client filename to parse
     * @return A newly allocated string containing the id of the client
     */
    char *GetClientID(const char *client_filename);
}

#endif //EXERCISE_II_CLIENT_UTILS_HPP
