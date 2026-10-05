#include <iostream>
#include <string>

class Player {
    private:
        std::string name;
        int health;
        int xp;
    public:
        std::string get_name() { return name; }
        int get_health() { return health; }
        int get_xp() { return xp; }
        void set_name(std::string n) { name = n; }
};

int main() {
    Player player1;
    player1.set_name("Arsenko");
    std::cout << "Player name: " << player1.get_name() << std::endl;
    return 0;
}