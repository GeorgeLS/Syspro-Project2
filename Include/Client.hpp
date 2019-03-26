#ifndef EXERCISE_II_CLIENT_HPP
#define EXERCISE_II_CLIENT_HPP

#include "Client_Parameters.hpp"
#include "Array.hpp"

class Client {
public:
    Client() = default;

    Client(const Client &client) = delete;

    Client(Client &&client) = delete;

    Client &operator=(const Client &client) = delete;

    Client &operator=(Client &&client) = delete;

    Client(int argc, char **argv);

    ~Client() = default;

    void Start();

private:
    Client_Parameters ParseClientParameters(int argc, char **argv);

    void ValidateParameters(Client_Parameters &program_arguments);

    void CreateIDFile();

    int argc_;
    char **argv_;
    Client_Parameters arguments_;
};


#endif //EXERCISE_II_CLIENT_HPP
