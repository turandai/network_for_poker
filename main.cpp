#include "Server.hpp"
#include <thread>

Server s1;

void server() {
    s1.run();
    //std::cout << "server thread end\n";
}

void input() {
    std::string s;
    while (true) {
        getline(std::cin, s);
        if (s != "") std::cout << s1.control(s) + "\n";
        if (s == "stop") return;
    }
}

int main() {
    //s1.run();
    
    std::string op = "start";
    while (true) {
        if (op == "help") {
            std::string temp;
            temp += "FAL Server Instructions\n";
            temp += "count:\n";
            temp += "    -c: count connected clients.\n";
            temp += "    -r: count totall rooms.\n";
            temp += "kill:\n";
            temp += "    -n [username]: kill a client by its name.\n";
            temp += "    -i [userIP]: kill all clients from this ip.\n";
            temp += "quit:\n";
            temp += "    quit this program(only avaliable when server is stopped).\n";
            temp += "show:\n";
            temp += "    -c: show details of every connected clients.\n";
            temp += "    -r: show details of all rooms.\n";
            temp += "stop:\n";
            temp += "    stop the server.\n";
            std::cout << temp << std::endl;
        }
        else if (op == "show -c" or op == "show -r" or op == "count -c" or op == "count -r" or (op.substr(0, 5) == "kill " and (op.substr(5, 3) == "-c " or op.substr(5, 3) == "-i ")))
            std::cout << "start server first\n";
        else if (op == "stop") std::cout << "server not started\n";
        else if (op == "quit") {
            std::cout << "bye\n";
            break;
        }
        else if (op == "start") {
            std::thread t1(server);
            std::thread t2(input);
            t1.join();
            t2.join();
        }
        std::cin >> op;
    }
    
    return 0;
}
