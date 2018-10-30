#ifndef Room_hpp
#define Room_hpp

#include "Client.hpp"
#include "jow/MathModel.hpp"
#include "jow/Player.hpp"



class Room {
public:
    std::vector<Client*> clients;
    MathModel jow;
    bool game;
    int landlord;
    std::string lscd[3];
    int lscdCount;
    
    Room();
    void add_lscd(std::string, int);
    bool is_full();
    bool is_empty();
    void new_game(); // inialize game variables
    void clear(Client);
    void add(Client*);
    void init();
    void endgame();
};

#endif
