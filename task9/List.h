#ifndef LIST_H
#define LIST_H

#include "IterableObject.h"
#include <string>
#include <iostream>
#include <type_traits>
#include <map>

template <typename T>
class List : virtual public IterableObject<T> {
protected:
    T* m_data;
    size_t m_size;
    size_t m_capacity;

    void resize(size_t new_capacity);

public:
    List();
    virtual ~List();

    void push_back(const T& value);
    void pop_back();
    void clear();
    size_t length() const override;
    T& operator[](size_t index);
    const T& operator[](size_t index) const;

    bool contains(const T& value) const override;



    std::string getType() const override;
    void print() const;
    void insert(size_t pos, const T& value);
    void erase(size_t pos);
};



template <typename T>
List<T>::List() : m_data(nullptr), m_size(0), m_capacity(0) {}

template <typename T>
List<T>::~List() { delete[] m_data; }

template <typename T>
void List<T>::resize(size_t new_capacity) {
    if (new_capacity <= m_capacity) return;
    T* new_data = new T[new_capacity];
    for (size_t i = 0; i < m_size; ++i) new_data[i] = m_data[i];
    delete[] m_data;
    m_data = new_data;
    m_capacity = new_capacity;
}

template <typename T>
void List<T>::push_back(const T& value) {
    if (m_size == m_capacity) {
        size_t new_cap = m_capacity == 0 ? 1 : m_capacity * 2;
        resize(new_cap);
    }
    m_data[m_size++] = value;
}

template <typename T>
void List<T>::pop_back() { if (m_size > 0) --m_size; }

template <typename T>
void List<T>::clear() { m_size = 0; }

template <typename T>
size_t List<T>::length() const { return m_size; }

template <typename T>
T& List<T>::operator[](size_t index) { return m_data[index]; }

template <typename T>
const T& List<T>::operator[](size_t index) const { return m_data[index]; }

template <typename T>
bool List<T>::contains(const T& value) const {
    for (size_t i = 0; i < m_size; ++i)
        if (m_data[i] == value) return true;
    return false;
}



template <typename T>
std::string List<T>::getType() const {
    if (std::is_same<T, int>::value) return "List<int>";
    if (std::is_same<T, double>::value) return "List<double>";
    if (std::is_same<T, std::string>::value) return "List<string>";
    return "List<T>";
}

template <typename T>
void List<T>::print() const {
    std::cout << "List contents (" << m_size << " elements): ";
    for (size_t i = 0; i < m_size; ++i) std::cout << m_data[i] << " ";
    std::cout << std::endl;
}

template <typename T>
void List<T>::insert(size_t pos, const T& value) {
    if (pos > m_size) pos = m_size;
    if (m_size == m_capacity) resize(m_capacity == 0 ? 1 : m_capacity * 2);
    for (size_t i = m_size; i > pos; --i) m_data[i] = m_data[i - 1];
    m_data[pos] = value;
    ++m_size;
}

template <typename T>
void List<T>::erase(size_t pos) {
    if (pos >= m_size) return;
    for (size_t i = pos; i < m_size - 1; ++i) m_data[i] = m_data[i + 1];
    --m_size;
}

#endif