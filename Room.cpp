#include "Room.hpp"

Room::Room() {
    game = false;
    landlord = -1;
    lscdCount = 0;
}

void Room::init() {
    lscdCount = 0;
    landlord = -1;
    game = false;
    for (int i = 0; i < clients.size(); i++) {
        clients[i]->user.init();
    }
    clients.clear();
}

void Room::add_lscd(std::string s, int id) {
    lscd[id] = s;
    lscdCount++;
}

void Room::endgame() {
    for (auto i = clients.begin(); i < clients.end(); i++) {
        (*i)->send("edgm");
    }
    std::cout << "Room " + std::to_string(clients[0]->user.room) + " game end\n";
    init();
}

void Room::new_game() {
    unsigned seed = (unsigned)time(0);
    srand(seed);
    clients[0]->send("seed" + std::to_string(seed));
    jow.dealtCards(rand() % 233);
    for (auto i = clients.begin(); i != clients.end(); i++) {
        (*i)->user.prep = false;
    }
    std::cout << "Room " + std::to_string(clients[0]->user.room) + " game begin\n";
}

bool Room::is_full() {
    return (clients.size() >= 3);
}

bool Room::is_empty() {
    return (clients.size() == 0);
}

void Room::clear(Client c) {
    clients.clear();
    game = false;
    std::cout << "Room " + std::to_string(c.user.room) + " game end\n";
}

void Room::add(Client* c) {
    clients.push_back(c);
}
