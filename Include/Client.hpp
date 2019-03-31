#ifndef EXERCISE_II_CLIENT_HPP
#define EXERCISE_II_CLIENT_HPP

#include "Client_Parameters.hpp"
#include "Array.hpp"
#include "Hash_Table.hpp"
#include "String_Utils.hpp"

using namespace Types;
using namespace String;

class Client {
public:
    Client() = delete;

    Client(int argc, char **argv);

    Client(const Client &client) = delete;

    Client(Client &&client) = delete;

    Client &operator=(const Client &client) = delete;

    Client &operator=(Client &&client) = delete;


    ~Client() = default;

    void Start();

private:
    using Table = Hash_Table<char *, bool, string_hash, equal_string>;

    Client_Parameters ParseClientParameters(int argc, char **argv);

    void ValidateParameters(Client_Parameters &program_arguments);

    void CreateIDFile();

    Array<char *> GetNewClients(const char *path);

    Table clients_map_{17};
    Client_Parameters arguments_{};
    char **argv_;
    int argc_;
    const unsigned int sleep_period_{1};
    bool stop_{false};
};


#endif //EXERCISE_II_CLIENT_HPP
