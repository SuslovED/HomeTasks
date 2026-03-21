#include <iostream>

int increment_counter() {
    static int counter = 0;
    return counter++;
}

struct Data {
    int value;
    Data() : value(42) {}
    Data(int v) : value(v) {}
    ~Data() { 
        increment_counter(); 
    }
    Data& operator=(int v) {
        value = v;
        return *this;
    }
    operator int() const { 
        return value; 
    }
};

class DataBuffer {
public:
    explicit DataBuffer(size_t size) : data(new Data[size]), sz(size) {}
    ~DataBuffer() { 
        delete[] data; 
    }

    // Запрещаем копирование
    DataBuffer(const DataBuffer&) = delete;
    DataBuffer& operator=(const DataBuffer&) = delete;

    // Разрешаем перемещение для std::swap
    DataBuffer(DataBuffer&& other) : data(other.data), sz(other.sz) {
        other.data = nullptr;
        other.sz = 0;
        ++move_construct_count;
    }

    DataBuffer& operator=(DataBuffer&& other) {
        if (this != &other) {
            delete[] data;
            data = other.data;
            sz = other.sz;
            other.data = nullptr;
            other.sz = 0;
            ++move_assign_count;
        }
        return *this;
    }

    size_t size() const { 
        return sz; 
    }

    Data& operator[](size_t i) { 
        return data[i]; 
    }

    const Data& operator[](size_t i) const { 
        return data[i]; 
    }

    static int get_move_construct_count() {
         return move_construct_count;
    }
    static int get_move_assign_count() {
         return move_assign_count;
    }
private:
    Data* data;
    size_t sz;
    static int move_construct_count;
    static int move_assign_count;
};

int DataBuffer::move_construct_count = 0;
int DataBuffer::move_assign_count = 0;

void print_buffer(const DataBuffer &buffer) {
    for (size_t i = 0; i < buffer.size(); i++) 
        std::cout << "buffer[" << i << "] == " << buffer[i] << std::endl;
}

int main()
{
    {
        DataBuffer buffer_1{5}, buffer_2{3};

        std::cout << "buffer_1.size() == " << buffer_1.size() << std::endl;
        std::cout << "buffer_2.size() == " << buffer_2.size() << std::endl;

        for (size_t i = 0; i < buffer_2.size(); i++) 
            buffer_2[i] = i + 1;

        std::cout << "buffer_1[0] == " << buffer_1[0].value << std::endl;
        std::cout << "buffer_2[0] == " << buffer_2[0].value << std::endl;

        std::swap(buffer_1, buffer_2);
        // DataBuffer buffer_copy = buffer_1; // forbidden!

        std::cout << "buffer_1.size() == " << buffer_1.size() << std::endl;
        std::cout << "buffer_2.size() == " << buffer_2.size() << std::endl;

        print_buffer(buffer_1);
    }
    std::cout << "~Data() called " << increment_counter() << " times" << std::endl;
    std::cout << "Move_constructors called " << DataBuffer::get_move_construct_count() << " times" << std::endl;
    std::cout << "Move_assignments called " << DataBuffer::get_move_assign_count() << " times" << std::endl;
    return 0;
}
