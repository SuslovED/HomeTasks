#ifndef STRINGLIST_H
#define STRINGLIST_H

#include "String.h"
#include "List.h"

class StringList : public String, public List<std::string> {
private:
    String m_ownString;        // своя строка
    List<std::string> m_ownList;  // свой список

public:
    StringList(const char* initial = "");
    ~StringList() = default;

    bool contains(const std::string& value) const override;
    size_t length() const override;
    std::string getType() const override;
    void print() const;
    
    void push_back(const std::string& value);
    void dropEven() override;
};

#endif