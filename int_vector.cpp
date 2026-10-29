#include <iostream>

class IntVector{
    private:
        int* data;
        size_t size;
        size_t capacity;
    public:
        IntVector(size_t initial_capacity = 4) : data(new int[initial_capacity]),
                                                size(0), capacity(initial_capacity){}
        ~IntVector()
        {
            delete[] data;
        }

        IntVector(const IntVector& other) : data(new int[other.get_capacity()]),
                                            size(other.get_size()), capacity(other.get_capacity())
        {
            for (size_t i = 0; i < other.get_size(); ++i){
                data[i] = other[i];
            }
        }
        
        IntVector& operator=(const IntVector& other)
        {
            if (this == &other){
                return *this;
            }
            delete[] data;

            size = other.size;
            capacity = other.capacity;
            data = new int[capacity];
            for (size_t i = 0; i < size; ++i) {
                data[i] = other.data[i];
            }
            return *this;
        }
        
        size_t get_capacity() const {return capacity;}
        void push_back(int value)
        {
            if (size == capacity){
                size_t new_capacity = capacity * 2;
                int* new_data = new int[new_capacity];
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

        int& operator[](size_t index)
        {
            return data[index];
        }

        const int& operator[](size_t index) const 
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

int main(){
    IntVector a(4);
    a.push_back(100);
    IntVector b(4);
    b.push_back(200);
    b = a;
    std::cout << "b[0]: " << b[0] << std::endl;
    return 0;
}