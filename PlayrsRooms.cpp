#include <memory>
#include <vector>
#include <iostream>
#include <algorithm>

class Entity{
    protected:
        std::string name;
        int hp;
    public:
        Entity(std::string n, int h) : name(n), hp(h) {}
        virtual ~Entity() = default;

        virtual std::string get_type(){
            return "Entity";
        };

        void attack(int dam){
            hp -= dam;
        }

        int get_hp() const{
            return hp;
        }
        std::string get_name() const{
            return name;
        }
};
class Player : public Entity{
    public:
        Player(std::string name) : Entity(name, 100){}
        
        std::string get_type() override{
            return "Player";
        }
};

class Monster : public Entity{
    public:
        Monster(std::string name) : Entity(name, 50){}
        
        std::string get_type() override{
            return "Monster";
        }
};

class Room{
    private:
        std::vector<std::unique_ptr<Entity>> entities;
    public:
        void add_entity(std::unique_ptr<Entity> entity){
            entities.push_back(std::move(entity));
        }
        void print_all_hp(){
            for (const auto& p : entities){
                std::cout << p->get_name() << " hp: " << p->get_hp() << '\n';
            }
        }
        void print_all_type(){
            for (const auto& p : entities){
                std::cout << p->get_name() << " type: " << p->get_type() << '\n';
            }
        }
        Entity* find_entity(const std::string& name){
            auto it = std::find_if(entities.begin(), entities.end(), [name](const auto& p){
                return p->get_name() == name;
            });
            if (it != entities.end()){
                return it->get();
            }
            else return nullptr;
        }
        void sort_by_hp(){
            std::sort(entities.begin(), entities.end(), [](const auto& a,const auto& b){
                return a->get_hp() > b->get_hp();
            });
        }
        void remove_dead(){
            std::erase_if(entities, [](const auto& p){
                return p->get_hp() <= 0;
            });
        }
};

int main()
{
    Room my_room;

    // Добавляем 3 игроков
    my_room.add_entity(std::make_unique<Player>("Arseniy"));
    my_room.add_entity(std::make_unique<Player>("Vova"));
    my_room.add_entity(std::make_unique<Monster>("5opka"));

    // Наносим разный урон через поиск
    Entity* bob = my_room.find_entity("Vova");
    if (bob) bob->attack(60); // останется 40 hp

    Entity* arseniy = my_room.find_entity("Arseniy");
    if (arseniy) arseniy->attack(10); // останется 90 hp

    std::cout << "--- ДО СОРТИРОВКИ ---\n";
    my_room.print_all_hp();
    my_room.print_all_type();

    // Сортируем: от большего HP к меньшему
    my_room.sort_by_hp();

    std::cout << "\n--- ПОСЛЕ СОРТИРОВКИ ПО HP ---\n";
    my_room.print_all_hp();
    my_room.print_all_type();

    return 0;
}