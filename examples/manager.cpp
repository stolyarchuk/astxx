#include <iostream>

#include "manager.h"

int main(int argc, char* argv[])
{
    if (argc != 4)
    {
        std::cerr << "Usage: " << argv[0] << " [host] [user] [password]" << std::endl;
        return EXIT_FAILURE;
    }

    std::string host_ = argv[1];
    std::string user_ = argv[2];
    std::string password_ = argv[3];

    try
    {
        using namespace astxx;
        manager::connection connection(host_);

        if (connection.is_connected())
            std::cout << "Connected to " << connection.name() << ", ver. " << connection.version() << std::endl;

        manager::action::login login(user_, password_);
        login(connection);
        connection(manager::action::logoff());
    }
    catch (const std::exception& e)
    {
        std::cerr << "Error: " << e.what() << std::endl;
        return EXIT_FAILURE;
    }

    return EXIT_SUCCESS;
}
