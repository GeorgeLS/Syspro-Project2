#include <iostream>
#include "../Include/Utils.hpp"
#include "../Include/Client.hpp"

int main(int argc, char **argv) {
    if (argc < 13) Utils::Usage();
    Client client{argc, argv};
    client.Start();
    return EXIT_SUCCESS;
}