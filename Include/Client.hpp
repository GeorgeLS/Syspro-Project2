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

    ~Client();

    void Start();

private:
    void SpawnProcesses(const char *client);

    using Table = Hash_Table<char *, bool, string_hash, equal_string>;

    Table clients_map_{17};
    Client_Parameters arguments_{};
    Array<char *> arguments_array_{};
    const unsigned int sleep_period_{1};
    bool stop_{};
};

#endif //EXERCISE_II_CLIENT_HPP
