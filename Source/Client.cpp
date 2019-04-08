#include <iostream>
#include <cinttypes>
#include <wait.h>
#include <fcntl.h>
#include "../Include/Client.hpp"
#include "../Include/Client_Utils.hpp"
#include "../Include/Process.hpp"
#include "../Include/Utils.hpp"
#include "../Include/File_Utils.hpp"

constexpr const char *SENDER_PATH = "./Sender";
constexpr const char *RECEIVER_PATH = "./Receiver";
constexpr const char *CLEANER_PATH = "./Cleaner";

using namespace Wrappers;
using namespace client;
using namespace File;

using _Profile_ = ClientProcessInfo::Profile;

// Add some lambda for the swag
auto GetProfileString = [](_Profile_ profile) {
    if (profile == _Profile_::Sender) {
        return "Sender";
    } else if (profile == _Profile_::Receiver) {
        return "Receiver";
    } else return "Cleaner";
};

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
    clients_map_[id_file] = {0U, true};
    while (!stop_) {
        sleep(sleep_period_);
        ResetClientMarks();
        Array<char *> clients = GetRegularFiles(arguments_.common_dir);
        for (size_t i = 0U; i != clients.Size(); ++i) {
            char *client = clients[i];
            if (!clients_map_.Contains(client)) {
                clients_map_[client] = {0U, true};
                SpawnProcesses(client);
            } else {
                clients_map_[client].second = true;
                free(client);
            }
        }
        
        CleanupDeletedClients();
        
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
                ReportMessage("[Client %" PRIu64 "]: %s finished successfully for client %s",
                              arguments_.id, GetProfileString(info.profile_), info.clientID_);
                list.Remove(it);
            }
        }
    }
}

void Client::ResetClientMarks() {
    for (size_t i = 0U; i != clients_map_.Buckets(); ++i) {
        auto &list = clients_map_.ListAt(i);
        if (list.Size() == 0) continue;
        FOREACH (list) {
            bool &checked = (*it).second.second;
            checked = false;
        }
    }
}

void Client::CleanupDeletedClients() {
    for (size_t i = 0U; i != clients_map_.Buckets(); ++i) {
        auto &list = clients_map_.ListAt(i);
        if (list.Size() == 0) continue;
        FOREACH (list) {
            bool &checked = (*it).second.second;
            if (!checked) {
                SpawnProcess(_Profile_::Cleaner, (*it).first);
                list.Remove(it);
            }
        }
    }
}

void Client::SpawnProcess(_Profile_ profile, char *client) {
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
        case _Profile_::Cleaner: {
            arguments_array_[0] = const_cast<char *>(CLEANER_PATH);
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
    ReportError("[Client %" PRIu64 "]: %s for client %s failed",
                arguments_.id, GetProfileString(info.profile_), info.clientID_);
    if (clients_map_.Contains(info.clientID_)) {
        uint &retries = clients_map_[info.clientID_].first;
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
    DeleteDirectory(arguments_.mirror_dir);
    free(id_file_path);
    stop_ = true;
}