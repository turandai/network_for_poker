#include "Server.hpp"
#include "User.hpp"
#include <cstring>
#include <sstream>
int main() {
    Server s1;
    s1.run();
    s1.stop();
    
    return 0;
}
