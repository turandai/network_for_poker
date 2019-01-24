#include "Server.hpp"
#include <thread>

Server::Server(int port) {
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
    
    server.sin_port = htons(60000);
    std::cout << "Server initializing...";
    while (bind(serverSocket, (sockaddr*)&server, sizeof(server)));
    std::cout << "done\n";
    
    //new_game();
}

bool Server::run() {
    std::cout << "Server starting...done\n";
    status = true;
    listen(serverSocket, 10);
    socklen_t len = sizeof(clientTemp);
    // running loop
    while (status) {
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
            if (name == "" or pwd == "") {
                c->send(type + "empty username or password");
                std::cout << "    empty username or password\n";
                return true;
            }
            bool exist = false;
            for (auto i = allClients.begin(); i != allClients.end(); i++) {
                if ((*i)->user.name == name) exist = true;
            }
            if (exist) {
                //c->send(type + "fail");
                c->send(type + name + " already logged in");
                std::cout << "    " + name + " already logged in\n";
            }
            else {
                std::string findResult = userControl.find(name, pwd);
                if (findResult == "done") {
                    c->user = userControl.get(name);
                    c->send(type + "done");
                    std::cout << "    " + name + " online\n";
                }
                else if (findResult == "wpsd") {
                    c->send(type + "wrong password");
                    std::cout << "    wrong password for " + name + "\n";
                }
                else if (findResult == "ntfd") {
                    c->send(type + "user not found");
                    std::cout << "    user " + name + " not found\n";
                }
                //c->send(type + findResult);
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
                //c->send(type + "bglt"); // name should begin with letter
                c->send(type + "name should begin with letters");
                std::cout << "    invalid name: " + name + "\n";
                check = false;
            }
            else {
                for (int i = 0; i < s.length(); i++) {
                    if (s[i] == ' ') {
                        //c->send(type + "nosp"); // name can't contain space
                        c->send(type + "name can't contain space");
                        std::cout << "    invalid name: " + name + "\n";
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
                        //c->send(type + "done");
                        c->send(type + "sign up done");
                        std::cout << "    " + name + "created\n";
                    }
                    else {
                        //c->send(type + "exis"); // user name already exists
                        c->send(type + "user already exist");
                        std::cout << "    " + name + " already exist\n";
                    }
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
    shutdown(serverSocket, SHUT_RDWR);
    rooms.clear();
    allClients.clear();
}

std::string Server::control(std::string s) {
    std::string temp = "";
    if (s == "help") {
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
        return temp;
    }
    if (s == "count -c") return std::to_string(allClients.size());
    if (s == "count -r") return std::to_string(rooms.size());
    if (s == "show -c") {
        temp += " ______________________________________\n";
        temp += "| ip              name       room   id |\n";
        for (auto i = allClients.begin(); i != allClients.end(); i++) {
            temp += "| " + (*i)->ip;
            for (int j = 0; j < 16 - (*i)->ip.length(); j++) temp += " ";
            temp += (*i)->user.name;
            for (int j = 0; j < 11 - (*i)->user.name.length(); j++) temp += " ";
            temp += std::to_string((*i)->user.room);
            for (int j = 0; j < 7 - std::to_string((*i)->user.room).length(); j++) temp += " ";
            temp += std::to_string((*i)->user.id);
            for (int j = 0; j < 3 - std::to_string((*i)->user.id).length(); j++) temp += " ";
            temp += "|\n";
        }
        temp += " --------------------------------------";
        return temp;
    }
    if (s == "show -r") {
        temp += " ____________________________\n";
        temp += "| num       player      game |\n";
        for (int i = 0; i < rooms.size(); i++) {
            temp += "| ";
            temp += std::to_string(i) + "         ";
            temp += std::to_string(rooms[i].clients.size()) + "           ";
            temp += std::to_string(rooms[i].game) + "    ";
            temp += "|\n";
        }
        temp += " ----------------------------";
        return temp;
    }
    if (s == "stop") {
        stop();
        return "server stopped";
    }
    if (s == "quit") return "stop server first";
    if (s == "start") return "server already started";
    if (s.substr(0, 5) == "kill ") {
        bool in = false, found = false;
        if (s.substr(5, 3) == "-n ") {
            in = true;
            std::string name = s.substr(8, s.size() - 8);
            for (auto i = allClients.begin(); i < allClients.end(); i++) {
                if ((*i)->user.name == name) {
                    (*i)->status = false;
                    found = true;
                }
            }
            if (found) return name + " killed";
        }
        if (s.substr(5, 3) == "-i ") {
            in = true;
            std::string ip = s.substr(8, s.size() - 8);
            for (auto i = allClients.begin(); i < allClients.end(); i++) {
                if ((*i)->ip == ip) {
                    (*i)->status = false;
                    found = true;
                }
            }
            if (found) return ip + " killed";
        }
        if (in) return "no such a client";
    }
    return "command not found";
}

Server::~Server() {
    stop();
}



