#include "Server.hpp"

Server::Server() {
    memset(&server, 0, sizeof(server));
    fcntl(serverSocket, F_SETFL, fcntl(serverSocket, F_GETFL, 0) | O_NONBLOCK);
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    memset(&clientTemp, 0, sizeof(clientTemp));
    len = sizeof(clientTemp);
}

bool Server::run(int port) {
    server.sin_port = htons(port);
    std::cout << "Server starting...\n";
    while (bind(serverSocket, (sockaddr*)&server, sizeof(server))) {
        continue;
    }
    std::cout << "Server started successfully.\nWaiting for connections...\n";
    listen(serverSocket, 5);
    socklen_t len = sizeof(clientTemp);
    // running loop
    while (true) {
        connectSocketTemp = accept(serverSocket, (sockaddr*)&clientTemp, &len);
        // accept connection requests
        if (connectSocketTemp >= 0) {
            // wether it has already connected
            exist = false;
            for (std::vector<Client>::iterator i = clients.begin(); i != clients.end(); i++) {
                if (i->get_ip() == clientTemp.sin_addr.s_addr and i->get_port() == clientTemp.sin_port) {
                    exist = true;
                    break;
                }
            }
            if (exist) {
                std::cout << "Connection already exist: " << inet_ntoa(clientTemp.sin_addr) << ": " << ntohs(clientTemp.sin_port) << std::endl;
            }
            else {
                clients.push_back(*new Client(connectSocketTemp, clientTemp));
                std::cout << "Connected from: " << inet_ntoa(clientTemp.sin_addr) << " " << ntohs(clientTemp.sin_port) << std::endl;
            }
        }
        // respond to recieved data
        for (std::vector<Client>::iterator i = clients.begin(); i != clients.end(); i++) {
            if (!respond(*i)) {
                i->connect = false;
            }
        }
        filter();
    }
    return true;
}


bool Server::respond(Client c) {
    int len = (int)recv(c.get_socket(), c.buf, sizeof(c.buf), 0);
    /* value of len
       -1: recieve nothing or error occurred
       =0: client shut down the connection
       >0: length of recieved data
    */
    if (len > 0) {
        std::cout << c.buf << std::endl;
    }
    else if (len == 0) {
        return false;
    }
    return true;
}

void Server::filter() {
    int len = (int)clients.size(), i = 0;
    while (i < len) {
        if (!clients[i].connect) {
            std::cout << "Connection lost: " << clients[i].ip << ", " << clients[i].port << std::endl;
            clients.erase(clients.begin() + (i--));
            len--;
        }
        i++;
    }
}




