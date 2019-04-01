#include <iostream>
#include <cinttypes>
#include <wait.h>
#include "../Include/Client.hpp"
#include "../Include/Client_Utils.hpp"
#include "../Include/Process.hpp"

constexpr const char *SENDER_PATH = "./Sender";
constexpr const char *RECEIVER_PATH = "./Receiver";

using namespace Wrappers;
using namespace client;

Client::Client(int argc, char **argv)
        : arguments_{ParseClientParameters(argc, argv)},
          arguments_array_{argc + 2U} {
    ValidateParameters(arguments_);
    for (size_t i = 1U; i != argc; ++i) {
        arguments_array_[i] = argv[i];
    }
    arguments_array_[argc + 1U] = NULL;
}

Client::~Client() {
    system("rm -rf common_dir/1.id mirror_dir");
}

void Client::Start() {
    CreateIDFile(arguments_);
    char *id_file;
    asprintf(&id_file, "%" PRIu64 ".id", arguments_.id);
    clients_map_[id_file] = true;
    while (!stop_) {
        sleep(sleep_period_);
        Array<char *> clients = GetClients(arguments_.common_dir);
        if (clients.Size() != 0) {
            for (size_t i = 0U; i != clients.Size(); ++i) {
                char *client = clients[i];
                if (!clients_map_.Contains(client)) {
                    clients_map_[client] = true;
                    SpawnProcesses(client);
                }
                free(client);
            }
        }
        stop_ = true;
    }
    for (size_t i = 0U; i != 2U; ++i) {
        int status;
        pid_t pid = wait(&status);
        std::cout << "[Parent] PID: " << pid << std::endl;
    }
    free(id_file);
}

void Client::SpawnProcesses(const char *client) {
    arguments_array_[arguments_array_.Size() - 2] = const_cast<char *>(client);
    arguments_array_[0] = const_cast<char *>(SENDER_PATH);
    Process sender{SENDER_PATH, arguments_array_};
    arguments_array_[0] = const_cast<char *>(RECEIVER_PATH);
    Process receiver{RECEIVER_PATH, arguments_array_};
    sender.Spawn();
    receiver.Spawn();
}