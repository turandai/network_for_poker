#ifndef User_hpp
#define User_hpp

#include <fstream>
#include <iostream>
#include <sstream>
#include <cstring>
#include <vector>

class User {
public:
    std::string name, pwd;
    int money;
    int id = -1, room = -1;
    bool prep = false;
    User(std::string name = "", std::string pwd = "", int money = 0)
    : name(name), pwd(pwd), money(money) {}
    void init(std::string t = "") {
        if (t == "total") {
            name = "";
            pwd = "";
            money = 0;
        }
        id = -1;
        room = -1;
        prep = false;
    }
};

class UserControl {
private:
    std::vector<User> users;
    std::string location = "/Users/turan/Documents/study/programming/c/poker/poker/data/user_data";
public:
    
    UserControl();
    void show();
    std::string find(std::string, std::string);
    User get(std::string);
    void add(std::string, std::string);
    void write();
    void update_money(std::string, std::string);
    int get_money(std::string);
    ~UserControl();
    
};

#endif
