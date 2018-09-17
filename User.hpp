#ifndef User_hpp
#define User_hpp

#include <fstream>
#include <iostream>
#include <cstring>
#include <vector>

class User {
public:
    std::string name, pwd;
    int money;
    User(std::string name, std::string pwd, int money)
    : name(name), pwd(pwd), money(money) {}
};

class UserControl {
private:
    std::fstream file;
    std::vector<User> users;
    
public:
    UserControl();
    void show();
    std::string find(std::string, std::string);
    
};

#endif
