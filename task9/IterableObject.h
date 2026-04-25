#ifndef ITERABLEOBJECT_H
#define ITERABLEOBJECT_H

#include <string>

template <typename T>
class IterableObject {
public:
    virtual ~IterableObject() = default;
    virtual bool contains(const T& value) const = 0;
    virtual size_t length() const = 0;
    virtual std::string getType() const = 0;
};

#endif