#ifndef Transfer_hpp
#define Transfer_hpp

#include <iostream>
#include <vector>
#include <cstring>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/poll.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <sys/shm.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <thread>

class Transfer{
private:
    std::string ip;
    int port;
    
public:
    Transfer();
    bool send();
    std::string packMsg(std::string msg);
    
    
};

#endif
