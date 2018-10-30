#include "Server.hpp"

Server::Server() {
    memset(&server, 0, sizeof(server));
    fcntl(serverSocket, F_SETFL, fcntl(serverSocket, F_GETFL, 0) | O_NONBLOCK);
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = htonl(INADDR_ANY);
    int temp = 1;
    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, (void*)&temp, sizeof(temp));
    setsockopt(serverSocket, SOL_SOCKET, SO_NOSIGPIPE, (void*)&temp, sizeof(temp));
    memset(&clientTemp, 0, sizeof(clientTemp));
    len = sizeof(clientTemp);
    status = false;
    
    //new_game();
}

bool Server::run(int port) {
    server.sin_port = htons(port);
    std::cout << "Server starting...\n";
    while (bind(serverSocket, (sockaddr*)&server, sizeof(server))) {
        continue;
    }
    std::cout << "Server started successfully.\nWaiting for connections...\n";
    status = true;
    listen(serverSocket, 10);
    socklen_t len = sizeof(clientTemp);
    // running loop
    while (true) {
        connectSocketTemp = accept(serverSocket, (sockaddr*)&clientTemp, &len);
        if (connectSocketTemp >= 0) {
            allClients.push_back(new Client(connectSocketTemp, clientTemp));
            allClients.back()->send("cntd");
            std::cout << "Connected from: " << inet_ntoa(clientTemp.sin_addr) << std::endl;
        }
        for (auto i = allClients.begin(); i != allClients.end(); i++) {
            if (!respond(*i)) {
                (*i)->status = false;
            }
        }
        // filter out inactive clients
        filter();
    }
    return true;
}

bool Server::respond(Client* c) {
    int len = (int)recv(c->connectSocket, c->recvBuf, sizeof(c->recvBuf), 0);
    std::string s = c->recvBuf, type;
    /* value of len
       -1: recieve nothing or error occurred
       =0: client shut down the connection
       >0: length of recieved data
    */
    if (len > 0) {
        type = s.substr(0, 4);
        s = s.substr(4);
        std::string who = c->ip;
        if (c->user.name != "") who = c->user.name;
        std::cout << who << ": " + type + "\n";
        if (type == "lgin") {
            std::string name = s.substr(1, (int)s[0] - 48);
            s = s.substr(1 + (int)s[0] - 48);
            std::string pwd = s.substr(1, (int)s[0] - 48);
            if (name == "" or pwd == "") return true;
            bool exist = false;
            for (auto i = allClients.begin(); i != allClients.end(); i++) {
                if ((*i)->user.name == name) exist = true;
            }
            if (exist) {
                c->send(type + "fail");
            }
            else {
                std::string findResult = userControl.find(name, pwd);
                if (findResult == "done") {
                    c->user = userControl.get(name);
                    std::cout << "    " + name + " online\n";
                }
                c->send(type + findResult);
            }
        }
        else if (type == "lgot") {
            std::cout << "    " + c->user.name + " offline\n";
            c->user.init("total");
        }
        else if (type == "siup") {
            std::string name = s.substr(1, (int)s[0] - 48);
            s = s.substr(1 + (int)s[0] - 48);
            bool check = true;
            if ((name[0] < 'a' or name[0] > 'z') and (name[0] < 'A' or name[0] > 'Z')) {
                c->send(type + "bglt"); // name should begin with letter
                check = false;
            }
            else {
                for (int i = 0; i < s.length(); i++) {
                    if (s[i] == ' ') {
                        c->send(type + "nosp"); // name can't contain space
                        check = false;
                    }
                }
            }
            if (check) {
                std::string pwd = s.substr(1, (int)s[0] - 48);
                if (name == "" or pwd == "") c->send("invd"); // invalid name or password
                else {
                    std::string findResult = userControl.find(name, pwd);
                    if (findResult == "ntfd") {
                        userControl.add(name, pwd);
                        std::cout << "    " + name + "created\n";
                        c->send(type + "done");
                    }
                    else c->send(type + "exis"); // user name already exists
                }
            }
            
        }
        else if (type == "prep") {
            if (c->user.name == "") {
                std::cout << "Log in first\n";
                return true;
            }
            if (c->user.prep == true) {
                std::cout << "Already prepared\n";
                return true;
            }
            c->user.room = allocate();
            c->user.prep = true;
            //c->user.id = (int)rooms[c->user.room].clients.size();
            rooms[c->user.room].add(c);
            if (rooms[c->user.room].clients.size() == 3) {
                //jow.showSituation(3);
                rooms[c->user.room].new_game();
                std::string temp;
                for (int i = 0; i < 3; i++) {
                    rooms[c->user.room].clients[i]->user.id = i;
                    temp += rooms[c->user.room].clients[i]->user.name + " ";
                    temp += std::to_string(rooms[c->user.room].clients[i]->user.money) + " ";
                }
                for (int i =0; i < 3; i++) {
                    rooms[c->user.room].clients[i]->send(type + std::to_string(rooms[c->user.room].clients[i]->user.id) + " " + temp.substr(0, temp.length() - 1));
                    rooms[c->user.room].clients[i]->send("card" + rooms[c->user.room].jow.players[i].cardSetToString());
                    rooms[c->user.room].game = true;
                }
                rooms[c->user.room].landlord = rand() % 3;
                broadcast("csld" + std::to_string(rooms[c->user.room].landlord), c->user.room);
            }
        }
        else if (type == "uscd") {
            broadcast(type + s, c->user.room);
        }
        else if (type == "beld") {
            broadcast(type + s, c->user.room);
        }
        else if (type == "ldld") {
            broadcast("card" + rooms[c->user.room].jow.hiddenCards.cardSetToString(), c->user.room);
        }
        else if (type == "wins") {
            rooms[c->user.room].init();
        }
        else if (type == "scor") {
            userControl.update_money(c->user.name, s);
            userControl.write();
        }
        else if (type == "gtsc") {
            //userControl.update_money(c->user.name, "0");
            int sc = userControl.get_money(c->user.name);
            c->send(type + std::to_string(sc));
        }
        else if (type == "lscd") {
            rooms[c->user.room].add_lscd(s, c->user.id);
            if (rooms[c->user.room].lscdCount == 3) {
                for (int i = 0; i < 3; i++) {
                    broadcast("lscd" + rooms[c->user.room].lscd[i], c->user.room);
                }
            }
        }
    }
    else if (len == 0) {
        return false;
    }
    return true;
}

void Server::broadcast(std::string s, int roomNum) {
    if (roomNum == -1) {
        for (auto i = allClients.begin();
             i != allClients.end(); i++) {
            (*i)->send(s);
        }
        return;
    }
    for (auto i = rooms[roomNum].clients.begin();
         i != rooms[roomNum].clients.end(); i++) {
        (*i)->send(s);
    }
}


void Server::filter() {
    int len = (int)allClients.size(), i = 0;
    while (i < len) {
        if (!allClients[i]->status) {
            std::string temp = allClients[i]->ip;
            if (allClients[i]->user.name != "") temp = allClients[i]->user.name;
            std::cout << "Connection lost: " << temp << std::endl;
            if (allClients[i]->user.room != -1) {
                rooms[allClients[i]->user.room].endgame();
            }
            delete allClients[i];
            allClients.erase(allClients.begin() + (i--));
            len--;
        }
        i++;
    }
    /*
    len = (int)rooms.size();
    i = 0;
    while (i < len) {
        if (rooms[i].is_empty()) {
            std::cout << "Room " + std::to_string(i) + " closed\n";
            rooms.erase(rooms.begin() + i--);
            len--;
        }
        i++;
    }*/
}

int Server::allocate() {
    int res = -1;
    bool need = true;
    if (!(rooms.size() == 0)) {
        for (int i = 0; i < rooms.size(); i++) {
            need &= rooms[i].is_full();
            if (!rooms[i].is_full()) res = i;
        }
    }
    if (need) {
        rooms.push_back(*new Room());
        res = (int)rooms.size() - 1;
    }
    return res;
}

void Server::stop() {
    status = false;
    broadcast("sstp");
    rooms.clear();
    allClients.clear();
    std::cout << "Server stopped\n";
}

Server::~Server() {
    stop();
}



