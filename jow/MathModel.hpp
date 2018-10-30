#ifndef MATHMODEL_H
#define MATHMODEL_H
#include "Player.hpp"
#include <cstring>
#include <ctime>
#include <cstdlib>
#include <algorithm>

class MathModel
{
public:
    MathModel();
    virtual ~MathModel();
    void dealtCards(int randSeed=0);
    void display();
    static bool checkFollow(const cardSet &pre,const cardSet &now);
    cardSet table;
    void startLocalGame(unsigned int randSeed=0);
    void showSituation(int id);
    Player players[3];
    Player hiddenCards;
    std::string cardSetToString();
protected:
    
private:
    int landlordId;
    static int getRank(const cardSet &x);
    
    unsigned int rSeed;
    
};

#endif // MATHMODEL_H
