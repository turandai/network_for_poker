#ifndef Server_hpp
#define Server_hpp

#include "Client.hpp"

class Server {
private:
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    int connectSocketTemp;
    sockaddr_in server, clientTemp;
    std::vector<Client> clients;
    socklen_t len;
    bool exist = false;
    
    
public:
    Server();
    bool run(int = 60000); // run server
    bool respond(Client); // recieve data and respond
    void filter(); // delete invalid client sockets
    
    
};

#endif
