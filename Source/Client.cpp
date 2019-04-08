#include <iostream>
#include <cinttypes>
#include <wait.h>
#include <fcntl.h>
#include "../Include/Client.hpp"
#include "../Include/Client_Utils.hpp"
#include "../Include/Process.hpp"
#include "../Include/Utils.hpp"

constexpr const char *SENDER_PATH = "./Sender";
constexpr const char *RECEIVER_PATH = "./Receiver";

using namespace Wrappers;
using namespace client;

using _Profile_ = ClientProcessInfo::Profile;

Client::Client(int argc, char **argv)
        : arguments_{ParseClientParameters(argc, argv)},
          arguments_array_{argc + 3U} {
    ValidateParameters(arguments_);
    for (size_t i = 1U; i != argc; ++i) {
        arguments_array_[i] = argv[i];
    }
    arguments_array_[argc + 2U] = NULL;
}

void Client::Start() {
    RegisterHandlers();
    CreateLogFile(arguments_.log_file);
    CreateIDFile(arguments_);
    int log_fd = open(arguments_.log_file, O_WRONLY);
    if (log_fd == -1) {
        Die("Couldn't open log file! Terminating");
    }
    asprintf(&arguments_array_[arguments_array_.Size() - 2], "%d", log_fd);
    char *id_file;
    asprintf(&id_file, "%" PRIu64 ".id", arguments_.id);
    clients_map_[id_file] = 0U;
    while (!stop_) {
        sleep(sleep_period_);
        Array<char *> clients = GetRegularFiles(arguments_.common_dir);
        for (size_t i = 0U; i != clients.Size(); ++i) {
            char *client = clients[i];
            if (!clients_map_.Contains(client)) {
                clients_map_[client] = 0U;
                SpawnProcesses(client);
            } else {
                free(client);
            }
        }

        FOREACH (processes_to_restart_) {
            auto &value = *it;
            SpawnProcess(value.profile_, value.clientID_);
        }
        processes_to_restart_.Clear();
        WaitAllChildren();
    }
    free(id_file);
}

void Client::WaitAllChildren() {
    for (size_t i = 0U; i != process_map_.Buckets(); ++i) {
        auto &list = process_map_.ListAt(i);
        if (list.Size() == 0) continue;
        FOREACH (list) {
            auto &pid = (*it).first;
            auto &info = (*it).second;
            int status;
            pid_t res = waitpid(pid, &status, WNOHANG);
            if (res > 0) {
                ReportMessage("%s finished successfully for client %s",
                              info.profile_ == _Profile_::Sender
                              ? "Sender" : "Receiver", info.clientID_);
                list.Remove(it);
            }
        }
    }
}

void Client::SpawnProcess(ClientProcessInfo::Profile profile, char *client) {
    arguments_array_[arguments_array_.Size() - 3] = client;
    switch (profile) {
        case _Profile_::Sender: {
            arguments_array_[0] = const_cast<char *>(SENDER_PATH);
            break;
        }
        case _Profile_::Receiver: {
            arguments_array_[0] = const_cast<char *>(RECEIVER_PATH);
            break;
        }
    }
    Process process{arguments_array_[0], arguments_array_};
    process.Spawn();
    process_map_[process.PID()] = {client, profile};
}

void Client::SpawnProcesses(char *client) {
    SpawnProcess(_Profile_::Sender, client);
    SpawnProcess(_Profile_::Receiver, client);
}

void Client::RegisterHandlers() {
    struct sigaction action{};
    sigemptyset(&action.sa_mask);
    action.sa_flags = SA_SIGINFO;
    action.sa_sigaction = &HandleSignal;
    sigaction(SIGUSR1, &action, NULL);
    sigaction(SIGQUIT, &action, NULL);
    sigaction(SIGINT, &action, NULL);
}

void Client::Handle_SIGUSR1(pid_t sender_pid) {
    ClientProcessInfo &info = process_map_[sender_pid];
    ReportError("%s for client %s failed",
                info.profile_ == _Profile_::Sender
                ? "Sender" : "Receiver", info.clientID_);
    if (clients_map_.Contains(info.clientID_)) {
        uint &retries = clients_map_[info.clientID_];
        if (retries < 4) {
            ++retries;
            processes_to_restart_.Add_Back(info);
        }
    }
    process_map_.Remove(sender_pid);
}

void Client::Handle_SIGINT_SIGQUIT() {
    char *id_file_path;
    asprintf(&id_file_path, "%s/%" PRIu64 ".id", arguments_.common_dir, arguments_.id);
    remove(id_file_path);
    remove(arguments_.mirror_dir);
    free(id_file_path);
    stop_ = true;
}