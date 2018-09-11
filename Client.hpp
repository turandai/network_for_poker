#ifndef Client_hpp
#define Client_hpp

#include "Transfer.hpp"

class Client {
private:
    int connectSocket;
    sockaddr_in sockaddr;
    
public:
    std::string ip;
    int port;
    bool connect;
    
    char buf[1024];
    
    Client(int connectSocket, sockaddr_in sockaddr);
    
    in_addr_t get_ip();
    
    in_port_t get_port();
    
    int get_socket();
    
    
};

#endif
