#include "manager.h"
#include <vector>
#include <string>
#include <iostream>


void print_event(astxx::manager::message::event e)
{
    //std::cout << e.format();
    std::cout << e["Paused"] << "\n";
}

int main(int argc, char** argv)
{
    std::vector<std::string> args(argv, argv + argc);

    if (args.size() != 4)
    {
        std::cerr << "Usage: " << args[0] << " [host] [username] [secret]\n";
        return 1;
    }

    try
    {
        using namespace astxx;

        manager::connection connection(args[1]);
        std::cout << "Connected to " << connection.name() << " v" << connection.version() << std::endl;
        boost::signals2::scoped_connection c1 = connection.register_event("QueueMember", print_event);
        //boost::signals::scoped_connection c2 = connection.register_event("Queue", print_event);
        manager::action::login login(args[2], args[3]);
        login(connection);
        manager::action::queue_pause qpause("SIP/3008", true);
        manager::message::response resp = qpause(connection);
        std::cout << resp.format();
        manager::action::queue_status qstatus("SIP/3008");
        resp = qstatus(connection);
        std::cout << resp.format();
        manager::action::queue_pause qpause2("SIP/3008", false);
        resp = qpause2(connection);
        std::cout << resp.format();
        manager::action::queue_status qstatus2("SIP/3008");
        resp = qstatus2(connection);
        std::cout << resp.format();

        for (;;)
        {
            connection.wait_event();
            connection.pump_messages();
            connection.process_events();
        }

        return 0;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    return 1;
}
