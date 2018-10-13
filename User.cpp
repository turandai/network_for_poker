#include "User.hpp"

UserControl::UserControl() {
    std::ifstream file(location);
    std::string nameTemp, pwdTemp;
    int moneyTemp;
    while (file.is_open() and !file.eof()) {
        file >> nameTemp >> pwdTemp >> moneyTemp;
        users.push_back(*new User(nameTemp, pwdTemp, moneyTemp));
    }
    users.pop_back();
    
}


void UserControl::show() {
    for (std::vector<User>::iterator i = users.begin(); i < users.end(); i++) {
        std::cout << i->name << ": " << i->money << std::endl;
    }
}

std::string UserControl::find(std::string name, std::string pwd) {
    for (std::vector<User>::iterator i = users.begin(); i < users.end(); i++) {
        if (i->name == name) {
            if (i->pwd == pwd) {
                return "Welcome";
            }
            else {
                return "Wrong password";
            }
        }
    }
    return "User not found";
}

void UserControl::add(std::string name, std::string pwd) {
    users.push_back(*new User(name, pwd, 0));
    std::ofstream file(location);
    for (std::vector<User>::iterator i = users.begin(); i < users.end(); i++) {
        file << i->name << " " << i->pwd << " " << i->money << "\n";
    }
}

User UserControl::get(std::string name) {
    for (std::vector<User>::iterator i = users.begin(); i < users.end(); i++) {
        if (i->name == name) return *i;
    }
    return *(new User);
}

UserControl::~UserControl() {
    std::ofstream file(location);
    for (std::vector<User>::iterator i = users.begin(); i < users.end(); i++) {
        file << i->name << " " << i->pwd << " " << i->money << "\n";
    }
}
