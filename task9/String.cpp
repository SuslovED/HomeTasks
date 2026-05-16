#include "String.h"
#include <iostream>

String::String(const char* str) {
    m_length = std::strlen(str);
    m_capacity = m_length + 1;
    m_data = new char[m_capacity];
    std::strcpy(m_data, str);
}

String::String(const String& other) {
    m_length = other.m_length;
    m_capacity = other.m_capacity;
    m_data = new char[m_capacity];
    std::strcpy(m_data, other.m_data);
}

String& String::operator=(const String& other) {
    if (this == &other) return *this;
    delete[] m_data;
    m_length = other.m_length;
    m_capacity = other.m_capacity;
    m_data = new char[m_capacity];
    std::strcpy(m_data, other.m_data);
    return *this;
}

String::~String() {
    delete[] m_data;
}

void String::resize(size_t new_capacity) {
    if (new_capacity <= m_capacity) return;
    char* new_data = new char[new_capacity];
    std::strcpy(new_data, m_data);
    delete[] m_data;
    m_data = new_data;
    m_capacity = new_capacity;
}

size_t String::length() const { return m_length; }

void String::append(const char* str) {
    size_t add_len = std::strlen(str);
    if (m_length + add_len + 1 > m_capacity) {
        resize((m_length + add_len + 1) * 2);
    }
    std::strcat(m_data, str);
    m_length += add_len;
}

void String::append(const String& other) { append(other.m_data); }

void String::clear() {
    m_data[0] = '\0';
    m_length = 0;
}

bool String::empty() const { return m_length == 0; }

size_t String::find(const char* substr) const {
    const char* pos = std::strstr(m_data, substr);
    return (pos == nullptr) ? std::string::npos : (pos - m_data);
}

bool String::contains(const std::string& substr) const {
    return find(substr.c_str()) != std::string::npos;
}

std::string String::getType() const { return "String"; }

void String::print() const { std::cout << m_data << std::endl; }

char String::operator[](size_t index) const {
    if (index >= m_length) throw std::out_of_range("String index out of range");
    return m_data[index];
}

String String::substr(size_t pos, size_t len) const {
    if (pos >= m_length) return String("");
    if (pos + len > m_length) len = m_length - pos;
    char* sub = new char[len + 1];
    std::strncpy(sub, m_data + pos, len);
    sub[len] = '\0';
    String result(sub);
    delete[] sub;
    return result;
}

void String::dropEven() {
    if (m_length == 0) return;

    char* new_data = new char[m_capacity];
    size_t new_len = 0;

    for (size_t i = 0; i < m_length; ++i) {
        if (i % 2 == 0) {
            new_data[new_len] = m_data[i];
            ++new_len;
        }
    }
    new_data[new_len] = '\0';

    for (size_t i = 0; i <= new_len; ++i) {
        m_data[i] = new_data[i];
    }
    m_length = new_len;

    delete[] new_data;
}
