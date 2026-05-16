#include <iostream>
#include <array>
#include <cstdint>
#include <stdexcept>
#include <type_traits>

class BlackHole {
private:
    size_t mass;

public:
    explicit BlackHole(size_t mass) : mass(mass) {}

    size_t get_mass() const {
        return mass;
    }

    template <typename T>
    void consume(T arg) {
        if constexpr (!std::is_pointer_v<T>) {
            throw std::runtime_error("Black hole has tried to consume non-pointer");
        } else {
            using Pointee = std::remove_pointer_t<T>;

            if constexpr (std::is_void_v<std::remove_cv_t<Pointee>>) {
                throw std::runtime_error("Black hole has tried to consume void");
            } else {
                if (arg == nullptr) {
                    std::cout << "There is nothing to consume" << std::endl;
                    return;
                }

                if (static_cast<const void*>(arg) == static_cast<const void*>(this)) {
                    throw std::runtime_error("Black hole has tried to consume itself");
                }

                size_t consumed_mass;
                if constexpr (std::is_same_v<std::remove_cv_t<Pointee>, BlackHole>) {
                    consumed_mass = arg->get_mass();
                } else {
                    consumed_mass = sizeof(Pointee);
                }

                std::cout << "Black hole consumed " << consumed_mass << ' '
                          << (std::is_const_v<Pointee> ? "const" : "non-const")
                          << " bytes" << std::endl;

                mass += consumed_mass;
                delete arg;
            }
        }
    }
};

int main()
{
    BlackHole hole(100);
    std::cout << "The initial mass is " << hole.get_mass() << " bytes" << std::endl;

    hole.consume(new int64_t);
    hole.consume(const_cast<const int64_t *>(new int64_t));
    hole.consume(new int32_t);
    hole.consume(static_cast<char *>(nullptr));
    hole.consume(new BlackHole{50});
    hole.consume(new std::array<char, 64>);

    try {
        hole.consume(5);
    } catch (std::runtime_error &err) {
        std::cout << "Error: " << err.what() << std::endl;
    }

    try {
        hole.consume(&hole);
    } catch (std::runtime_error &err) {
        std::cout << "Error: " << err.what() << std::endl;
    }

    try {
        hole.consume((void *) nullptr);
    } catch (std::runtime_error &err) {
        std::cout << "Error: " << err.what() << std::endl;
    }

    std::cout << "The total mass is: " << hole.get_mass() << " bytes" << std::endl;

    return 0;
}
