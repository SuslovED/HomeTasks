#include "StringList.h"
#include <iostream>
#include <cstring>

StringList::StringList(const char* initial) 
    : String("")
    , List<std::string>()
    , m_ownString(initial)
    , m_ownList()
{
    if (std::strlen(initial) > 0) {
        m_ownList.push_back(std::string(initial));
    }
}

bool StringList::contains(const std::string& value) const {
    for (size_t i = 0; i < m_ownList.length(); ++i) {
        const std::string& elem = m_ownList[i];
        if (elem == value || elem.find(value) != std::string::npos) {
            return true;
        }
    }
    return false;
}

size_t StringList::length() const {
    return m_ownList.length();
}

std::string StringList::getType() const { 
    return "StringList"; 
}

void StringList::print() const {
    std::cout << "String part: ";
    m_ownString.print();
    std::cout << "List part:   ";
    m_ownList.print();
}

void StringList::push_back(const std::string& value) {
    m_ownList.push_back(value);
}

void StringList::dropEven() {
    m_ownList.dropEven();
    
    for (size_t i = 0; i < m_ownList.length(); ++i) {
        std::string word = m_ownList[i];
        std::string new_word;
        for (size_t j = 0; j < word.length(); ++j) {
            if (j % 2 == 0) {
                new_word += word[j];
            }
        }
        m_ownList[i] = new_word;
    }
}