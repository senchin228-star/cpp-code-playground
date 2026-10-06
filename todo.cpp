#include <string>
#include <iostream>
#include <vector>

struct todo {
    std::string name, status;
};

int main()
{
    std::vector<todo> todos;
    while(true){
        std::string name;
        std::cout << "Enter your task (Or exit)\n";
        std::getline(std::cin, name);
        if (name == "exit"){
            break;
        }
        
        todos.push_back({name, "New"});
        
        std::cout << "Added\n";
    }

    std::cout << "All tasks:\n";
    for (size_t i = 0; i < todos.size(); ++i){
        std::cout << "Task: " << todos[i].name << ", Status: " << todos[i].status << '\n';
    }

    return 0;
}