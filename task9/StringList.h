#ifndef STRINGLIST_H
#define STRINGLIST_H

#include "String.h"
#include "List.h"

class StringList : public String, public List<std::string> {
public:
    StringList(const char* initial = "");
    ~StringList() override;

    bool contains(const std::string& value) const override;
    size_t length() const override;
    std::string getType() const override;
    void print() const;

    
    using List<std::string>::push_back;
};

#endif