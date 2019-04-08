#include "../Include/Client.hpp"
#include "../Include/Utils.hpp"
#include "../Include/Client_Utils.hpp"

int main(int argc, char **argv) {
    if (argc < 13) Utils::Usage();
    Client client{argc, argv};
    global_client = &client;
    client.Start();
    return EXIT_SUCCESS;
}