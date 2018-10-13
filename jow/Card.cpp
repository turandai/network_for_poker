#include "Card.hpp"
Card::Card()
{
    //ctor
}

unsigned int Card::makeId(int suit,int type)
{
    
    if(type==14) return 53;
    else if(type==15) return 54;
    
    return (type-1)*4+suit;
}


Card::Card(unsigned int id) : id(id)
{
    suit = (id-1)%4+1;
    num = (id+3)/4;
    if(id==53)
    {
        suit = 5;
        num = 14;
    }
    else if(id==54)
    {
        suit = 6;
        num=15;
    }
    //ctor
}
void Card::showInTerminal() const
{
    switch(id)
    {
        case 53://
            //SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE),BACKGROUND_INTENSITY|FOREGROUND_RED|
            //            FOREGROUND_GREEN|FOREGROUND_BLUE);
            std::cout<<"0W ";
            break;
        case 54:
            std::cout<<"1W ";
            break;
        default:
            std::cout<<DIS[num]<<" ";
    }
}
