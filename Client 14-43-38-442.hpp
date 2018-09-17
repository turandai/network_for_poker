#ifndef Client_hpp
#define Client_hpp

#include "Transfer.hpp"

class Client {
private:
    int connectSocket;
    sockaddr_in sockaddr, server;
    
public:
    std::string ip;
    int port;
    bool status;
    Transfer transfer;
    char recvBuf[1024];
    
    Client();
    
    Client(int connectSocket, sockaddr_in sockaddr);
    
    
    bool connect(std::string, int = 60000);
    
    bool send(std::string);
    
    in_addr_t get_ip();
    
    in_port_t get_port();
    
    int get_socket();
    
    
};

#endif
