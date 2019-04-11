#ifndef EXERCISE_II_CLIENT_HPP
#define EXERCISE_II_CLIENT_HPP

#include "Client_Parameters.hpp"
#include "Array.hpp"
#include "Hash_Table.hpp"
#include "String_Utils.hpp"
#include "ClientProcessInfo.hpp"

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

    void Handle_SIGUSR1(pid_t sender_pid);

    void Handle_SIGINT_SIGQUIT();

private:
    void SpawnProcess(ClientProcessInfo::Profile profile, char *client, uint retries_so_far);

    void SpawnProcesses(char *client);

    void RegisterHandlers();

    void WaitAllChildren();

    void ResetClientMarks();

    void CleanupDeletedClients();

    using ClientTable = Hash_Table<char *, bool, string_hash, equal_string>;

    using ProcessTable = Hash_Table<pid_t, ClientProcessInfo>;

    ClientTable clients_map_{103};
    ProcessTable process_map_{211};
    List<ClientProcessInfo> processes_to_restart_{};

    Client_Parameters arguments_{};
    Array<char *> arguments_array_{};
    const unsigned int sleep_period_{1};
};

extern Client *global_client;

#endif //EXERCISE_II_CLIENT_HPP
