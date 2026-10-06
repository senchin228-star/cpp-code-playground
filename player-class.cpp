#include <iostream>
#include <string>

class Player {
    private:
        std::string name;
        int health;
        int xp;
    public:
        std::string get_name() const { return name; }
        int get_health() const { return health; }
        int get_xp() const { return xp; }
        Player(const std::string& name_val = "None", int health_val = 100, int xp_val = 0)
            : name{name_val}, health{health_val}, xp{xp_val} {
        }
};

int main() {
    Player player1("Arsenko", 100, 42);
    std::cout << "Player name: " << player1.get_name() << '\n';
    std::cout << "Player health: " << player1.get_health() << '\n';
    std::cout << "Player XP: " << player1.get_xp() << '\n';
    return 0;
}