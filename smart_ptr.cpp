#include <iostream>
template <typename T>

class UniquePtr{
    private:
        T* ptr;
    public:
        UniquePtr(T* p = nullptr) : ptr(p){}
        ~UniquePtr(){
            delete ptr;
        }
        UniquePtr(const UniquePtr&) = delete;
        UniquePtr operator=(const UniquePtr&) = delete;
        UniquePtr(UniquePtr&& other) noexcept {
            ptr = other.ptr;
            other.ptr = nullptr;
        }
        T& operator*() const {
            return *ptr;
        }
        T* operator->() const {
            return ptr;
        }
        T* get() const{
            return ptr;
        }
};
struct Player {
    int hp = 100;
    void take_damage(int dmg) { hp -= dmg; }
};

int main() {
    UniquePtr<Player> p1(new Player());
    p1->take_damage(20);
    std::cout << "HP: " << p1->hp << std::endl; // выведет 80


    UniquePtr<Player> p2 = std::move(p1); // Передали владение в p2!
    std::cout << "HP p2: " << p2->hp << std::endl;

    return 0;
}