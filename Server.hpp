#ifndef Server_hpp
#define Server_hpp

#include "Room.hpp"

class Server {
private:
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    int connectSocketTemp;
    sockaddr_in server, clientTemp;
    socklen_t len;
    bool status;
    UserControl userControl;
    Room r1;
    std::vector<Room> rooms;
    std::vector<Client*> allClients;
    int roomNow;
    
    bool respond(Client*);
    void broadcast(std::string, int = -1); // broadcast msg to a room
    void filter(); // delete invalid client sockets
    void new_game(); // inialize game variables
    int allocate();
    
public:
    Server(int = 60000);
    bool run();// recieve data and respond
    std::string control(std::string = "");
    void stop();
    ~Server();
    
};

#endif
