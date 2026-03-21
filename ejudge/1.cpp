#include <iostream>

class Reader {
public:
    Reader() {
        if (!(std::cin >> value)) {
            valid = false;
        } else {
            valid = true;
        }
    }

    ~Reader() {
        if (valid) {
            static bool first = true;
            if (!first) {
                std::cout << ' ';
            } else {
                first = false;
            }
            std::cout << value;
        }
    }

    bool is_valid() const {
        return valid;
    }

private:
    unsigned long long value {};
    bool valid {};
};

void recursive_reading() {
    Reader r;
    if (r.is_valid()) {
        recursive_reading();
    }
}

int main() {
    recursive_reading();
    return 0;

}
