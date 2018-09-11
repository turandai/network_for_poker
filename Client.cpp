#include "Client.hpp"

Client::Client(int connectSocket, sockaddr_in sockaddr) {
    this->sockaddr = sockaddr;
    this->connectSocket = connectSocket;
    ip = inet_ntoa(sockaddr.sin_addr);
    port = ntohs(sockaddr.sin_port);
    memset(buf, 0, sizeof(buf));
    connect = true;
    
}

in_addr_t Client::get_ip() {
    return sockaddr.sin_addr.s_addr;
}

in_port_t Client::get_port() {
    return sockaddr.sin_port;
}

int Client::get_socket(){
    return connectSocket;
}
