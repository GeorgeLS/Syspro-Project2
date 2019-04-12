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

int log_fd = -1;

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
    log_fd = open(arguments_.log_file, O_WRONLY);
    if (log_fd == -1) {
        Die("Couldn't open log file! Terminating");
    }
    FileReport(log_fd, "[C] %" PRIu64 "\n", arguments_.id);
    asprintf(&arguments_array_[arguments_array_.Size() - 2], "%d", log_fd);
    char *id_file;
    asprintf(&id_file, "%" PRIu64 ".id", arguments_.id);
    clients_map_[id_file] = true;
    while (true) {
        sleep(sleep_period_);
        ResetClientMarks();
        Array<char *> clients = GetRegularFiles(arguments_.common_dir);
        for (size_t i = 0U; i != clients.Size(); ++i) {
            char *client = clients[i];
            if (!clients_map_.Contains(client)) {
                clients_map_[client] = true;
                SpawnProcesses(client);
            } else {
                clients_map_[client] = true;
                free(client);
            }
        }
        
		WaitAllChildren();
		CleanupDeletedClients();
		SpawnProcessesToRestart();
    }
}

void Client::WaitAllChildren() {
    for (size_t i = 0U; i != process_map_.Buckets(); ++i) {
        auto &list = process_map_.ListAt(i);
        FOREACH (list) {
            auto &pid = (*it).first;
            auto &info = (*it).second;
            int status;
            pid_t res = waitpid(pid, &status, WNOHANG);
            if (res > 0) {
				char *clientID = GetClientID(info.clientID_);
                if (status == EXIT_SUCCESS) {
                    ReportMessage("[Client %" PRIu64 "]: %s finished successfully for client %s",
                                  arguments_.id, GetProfileString(info.profile_), clientID);
                } else if (status == EXIT_FAILURE) {
					ReportError("[Client %" PRIu64 "]: %s for client %s failed",
								arguments_.id, GetProfileString(info.profile_), clientID);
				}
				free(clientID);
				list.Remove(it);
			}
		}
	}
}

void Client::ResetClientMarks() {
	for (size_t i = 0U; i != clients_map_.Buckets(); ++i) {
		auto &list = clients_map_.ListAt(i);
		FOREACH (list) {
			bool &checked = (*it).second;
			checked = false;
		}
	}
}

void Client::CleanupDeletedClients() {
	for (size_t i = 0U; i != clients_map_.Buckets(); ++i) {
		auto &list = clients_map_.ListAt(i);
		FOREACH (list) {
			bool &checked = (*it).second;
			if (!checked) {
				SpawnProcess(_Profile_::Cleaner, (*it).first, 0);
				list.Remove(it);
			}
		}
	}
}

void Client::SpawnProcessesToRestart() {
	FOREACH (processes_to_restart_) {
		auto &value = *it;
		if (clients_map_.Contains(value.clientID_)) {
		    ReportError("Respawning process");
			SpawnProcess(value.profile_, value.clientID_, value.reties_);
		}
	}
	processes_to_restart_.Clear();
}

void Client::SpawnProcess(_Profile_ profile, char *client, uint retries_so_far) {
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
	process_map_[process.PID()] = {client, profile, retries_so_far};
}

void Client::SpawnProcesses(char *client) {
	SpawnProcess(_Profile_::Sender, client, 0);
	SpawnProcess(_Profile_::Receiver, client, 0);
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
	uint &retries = info.reties_;
	if (retries < 4) {
		++retries;
		processes_to_restart_.Add_Back(info);
	}
}

void Client::Handle_SIGINT_SIGQUIT() {
	if (log_fd != -1) {
		FileReport(log_fd, "[L] %" PRIu64 "\n", arguments_.id);
	}
	// Send SIGQUIT to all children currently running
	for (size_t i = 0U; i != process_map_.Buckets(); ++i) {
		auto &list = process_map_.ListAt(i);
		FOREACH (list) {
			auto &value = *it;
			kill(value.first, SIGQUIT);
		}
	}
	
	char *id_file_path;
	asprintf(&id_file_path, "%s/%" PRIu64 ".id", arguments_.common_dir, arguments_.id);
	remove(id_file_path);
	free(id_file_path);
	DeleteDirectory(arguments_.mirror_dir);
	this->~Client();
	exit(EXIT_SUCCESS);
}