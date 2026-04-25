#include "StringList.h"
#include <iostream>
#include <map>

StringList::StringList(const char* initial) : String(initial), List<std::string>() {
    if (std::strlen(initial) > 0) {
        push_back(std::string(initial));
    }
}

StringList::~StringList() = default;

bool StringList::contains(const std::string& value) const {
    for (size_t i = 0; i < List<std::string>::length(); ++i) {
        const std::string& elem = List<std::string>::operator[](i);
        if (elem == value || elem.find(value) != std::string::npos) {
            return true;
        }
    }
    return false;
}

size_t StringList::length() const {
    return List<std::string>::length();
}

std::string StringList::getType() const { 
    return "StringList"; 
}

void StringList::print() const {
    std::cout << "String part: "; String::print();
    std::cout << "List part: ";   List<std::string>::print();
}
