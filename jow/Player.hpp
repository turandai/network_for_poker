#ifndef PLAYER_H
#define PLAYER_H

#include "Card.hpp"
#include <sstream>
#include <set>
typedef  std::set<Card> cardSet;
class Player
{
public:
    Player();
    virtual ~Player();
    
    bool addCard(unsigned int id);
    bool addCard(cardSet x);
    bool eraseCard(unsigned int id);
    bool eraseCard(cardSet x);
    void clearCards();
    static std::string getCardType(const cardSet &x);
    static void showAllCard(cardSet &cards,int opt=0);
    int getCardsNum();
    
    void showAllCard(int opt=0);//1 for show id
    void showType();
    static cardSet stringToCardSet(std::string str);
    std::string CardSetToString();
    cardSet getCardSet();
    cardSet choseCard();
    cardSet cards;
protected:
    
private:
    unsigned int index;
    unsigned int cardsNum;
    
    
};

#endif // PLAYER_H
