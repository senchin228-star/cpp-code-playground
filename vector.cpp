#include <iostream>
#include <string>

template <typename T>
class Vector{
    private:
        T* data;
        size_t size;
        size_t capacity;
    public:
        Vector(size_t initial_capacity = 4) : data(new T[initial_capacity]),
                                                size(0), capacity(initial_capacity){}
        ~Vector()
        {
            delete[] data;
        }

        Vector(const Vector& other) : data(new T[other.capacity]),
                                            size(other.get_size()), capacity(other.capacity)
        {
            for (size_t i = 0; i < other.get_size(); ++i){
                data[i] = other[i];
            }
        }
        
        Vector& operator=(const Vector& other)
        {
            if (this == &other){
                return *this;
            }
            delete[] data;

            size = other.size;
            capacity = other.capacity;
            data = new T[capacity];
            for (size_t i = 0; i < size; ++i) {
                data[i] = other.data[i];
            }
            return *this;
        }
        
        void push_back(const T& value)
        {
            if (size == capacity){
                size_t new_capacity = capacity * 2;
                T* new_data = new T[new_capacity];
                for(size_t i = 0; i < size; ++i){
                    new_data[i] = data[i];
                }
                delete[] data;
                data = new_data;
                capacity = new_capacity;
            }
            data[size] = value;
            size++;
        }

        T& operator[](size_t index)
        {
            return data[index];
        }

        const T& operator[](size_t index) const 
        {
            return data[index];
        }

        int get(size_t index) const
        {
            return data[index];
        }

        size_t get_size() const
        {
            return size;
        }
};

int main() {
    // Вектор чисел
    Vector<int> numbers;
    numbers.push_back(10);
    numbers.push_back(20);
    // Вектор строк!
    Vector<std::string> strings;
    strings.push_back("Hello");
    strings.push_back("dexter morgan");
    for (size_t i = 0; i < strings.get_size(); ++i) {
        std::cout << strings[i] << " ";
    }
    for (size_t i = 0; i < numbers.get_size(); ++i) {
        std::cout << numbers[i] << " ";
    }
    std::cout << std::endl;
    return 0;
}