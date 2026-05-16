#ifndef STRING_H
#define STRING_H

#include "IterableObject.h"
#include <string>
#include <stdexcept>
#include <cstring>

class String : virtual public IterableObject<std::string> {
protected:
    char* m_data;
    size_t m_length;
    size_t m_capacity;

    void resize(size_t new_capacity);

public:
    String(const char* str = "");
    String(const String& other);
    String& operator=(const String& other);
    virtual ~String();

    size_t length() const override;
    void append(const char* str);
    void append(const String& other);
    void clear();
    bool empty() const;
    size_t find(const char* substr) const;
    bool contains(const std::string& substr) const override;
    std::string getType() const override;
    void print() const;
    char operator[](size_t index) const;
    String substr(size_t pos, size_t len) const;

    void dropEven() override;
};

#endif