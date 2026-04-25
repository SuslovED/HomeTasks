#include <iostream>
#include <vector>
#include <string>
#include "String.h"
#include "List.h"
#include "StringList.h"

// ============= ГЛОБАЛЬНЫЕ ХРАНИЛИЩА =============
std::vector<IterableObject<std::string>*> stringObjects;
std::vector<IterableObject<int>*> intObjects;
std::vector<IterableObject<double>*> doubleObjects;

// ============= ВСПОМОГАТЕЛЬНЫЕ ФУНКЦИИ =============

void printSeparator() {
    std::cout << "\n========================================\n";
}

void showAll() {
    printSeparator();
    
    std::cout << "\n=== СТРОКОВЫЕ ОБЪЕКТЫ ===\n";
    if (stringObjects.empty()) {
        std::cout << "Нет объектов\n";
    } else {
        for (size_t i = 0; i < stringObjects.size(); ++i) {
            std::cout << "\n[" << i+1 << "] " << stringObjects[i]->getType() 
                      << " | length=" << stringObjects[i]->length() << "\n";
            
            String* s = dynamic_cast<String*>(stringObjects[i]);
            if (s) {
                std::cout << "    Строка: ";
                s->print();
            }
            
            List<std::string>* l = dynamic_cast<List<std::string>*>(stringObjects[i]);
            if (l) {
                std::cout << "    Список: ";
                l->print();
            }
        }
    }
    
    std::cout << "\n=== LIST<INT> ===\n";
    if (intObjects.empty()) {
        std::cout << "Нет объектов\n";
    } else {
        for (size_t i = 0; i < intObjects.size(); ++i) {
            List<int>* l = dynamic_cast<List<int>*>(intObjects[i]);
            std::cout << "[" << i+1 << "] " << intObjects[i]->getType() 
                      << " | length=" << intObjects[i]->length() << "\n";
            if (l) {
                std::cout << "    Содержимое: ";
                l->print();
            }
        }
    }
    
    std::cout << "\n=== LIST<DOUBLE> ===\n";
    if (doubleObjects.empty()) {
        std::cout << "Нет объектов\n";
    } else {
        for (size_t i = 0; i < doubleObjects.size(); ++i) {
            List<double>* l = dynamic_cast<List<double>*>(doubleObjects[i]);
            std::cout << "[" << i+1 << "] " << doubleObjects[i]->getType() 
                      << " | length=" << doubleObjects[i]->length() << "\n";
            if (l) {
                std::cout << "    Содержимое: ";
                l->print();
            }
        }
    }
    
    printSeparator();
}

void showMenu() {
    std::cout << "\n========== МЕНЮ ==========\n";
    std::cout << "1. Создать String\n";
    std::cout << "2. Создать List<string>\n";
    std::cout << "3. Создать StringList\n";
    std::cout << "4. Создать List<int>\n";
    std::cout << "5. Создать List<double>\n";
    std::cout << "6. Показать все объекты\n";
    std::cout << "7. Добавить элемент в объект\n";
    std::cout << "8. Проверить contains\n";
    std::cout << "9. Запустить тест\n";
    std::cout << "0. Выход\n";
    std::cout << "Выбор: ";
}

void addElement() {
    printSeparator();
    std::cout << "В какой объект добавить?\n";
    std::cout << "1. List<int>\n";
    std::cout << "2. List<double>\n";
    std::cout << "3. List<string>\n";
    std::cout << "4. StringList\n";
    std::cout << "Выбор: ";
    
    int type;
    std::cin >> type;
    
    if (type == 1) {
        if (intObjects.empty()) {
            std::cout << "Нет List<int>. Сначала создайте.\n";
            return;
        }
        size_t idx;
        std::cout << "Номер объекта (1-" << intObjects.size() << "): ";
        std::cin >> idx;
        if (idx < 1 || idx > intObjects.size()) {
            std::cout << "Неверно\n";
            return;
        }
        int val;
        std::cout << "Введите int: ";
        std::cin >> val;
        dynamic_cast<List<int>*>(intObjects[idx-1])->push_back(val);
        std::cout << "Добавлено\n";
    }
    else if (type == 2) {
        if (doubleObjects.empty()) {
            std::cout << "Нет List<double>. Сначала создайте.\n";
            return;
        }
        size_t idx;
        std::cout << "Номер объекта (1-" << doubleObjects.size() << "): ";
        std::cin >> idx;
        if (idx < 1 || idx > doubleObjects.size()) {
            std::cout << "Неверно\n";
            return;
        }
        double val;
        std::cout << "Введите double: ";
        std::cin >> val;
        dynamic_cast<List<double>*>(doubleObjects[idx-1])->push_back(val);
        std::cout << "Добавлено\n";
    }
    else if (type == 3) {
        if (stringObjects.empty()) {
            std::cout << "Нет объектов. Сначала создайте.\n";
            return;
        }
        size_t idx;
        std::cout << "Номер объекта (1-" << stringObjects.size() << "): ";
        std::cin >> idx;
        if (idx < 1 || idx > stringObjects.size()) {
            std::cout << "Неверно\n";
            return;
        }
        List<std::string>* l = dynamic_cast<List<std::string>*>(stringObjects[idx-1]);
        if (l) {
            std::string val;
            std::cout << "Введите строку: ";
            std::cin.ignore();
            std::getline(std::cin, val);
            l->push_back(val);
            std::cout << "Добавлено\n";
        } else {
            std::cout << "Это не List<string>\n";
        }
    }
    else if (type == 4) {
        if (stringObjects.empty()) {
            std::cout << "Нет объектов. Сначала создайте.\n";
            return;
        }
        size_t idx;
        std::cout << "Номер объекта (1-" << stringObjects.size() << "): ";
        std::cin >> idx;
        if (idx < 1 || idx > stringObjects.size()) {
            std::cout << "Неверно\n";
            return;
        }
        StringList* sl = dynamic_cast<StringList*>(stringObjects[idx-1]);
        if (sl) {
            std::string val;
            std::cout << "Введите строку: ";
            std::cin.ignore();
            std::getline(std::cin, val);
            sl->push_back(val);
            std::cout << "Добавлено\n";
        } else {
            std::cout << "Это не StringList\n";
        }
    }
}

void checkContains() {
    printSeparator();
    std::cout << "1. String/List<string>/StringList (поиск строки)\n";
    std::cout << "2. List<int> (поиск int)\n";
    std::cout << "3. List<double> (поиск double)\n";
    std::cout << "Выбор: ";
    
    int type;
    std::cin >> type;
    
    if (type == 1) {
        if (stringObjects.empty()) {
            std::cout << "Нет объектов\n";
            return;
        }
        size_t idx;
        std::cout << "Номер объекта (1-" << stringObjects.size() << "): ";
        std::cin >> idx;
        if (idx < 1 || idx > stringObjects.size()) {
            std::cout << "Неверно\n";
            return;
        }
        std::string search;
        std::cout << "Что ищем: ";
        std::cin.ignore();
        std::getline(std::cin, search);
        bool res = stringObjects[idx-1]->contains(search);
        std::cout << "Результат: " << (res ? "TRUE" : "FALSE") << "\n";
    }
    else if (type == 2) {
        if (intObjects.empty()) {
            std::cout << "Нет объектов\n";
            return;
        }
        size_t idx;
        std::cout << "Номер объекта (1-" << intObjects.size() << "): ";
        std::cin >> idx;
        if (idx < 1 || idx > intObjects.size()) {
            std::cout << "Неверно\n";
            return;
        }
        int search;
        std::cout << "Что ищем: ";
        std::cin >> search;
        bool res = intObjects[idx-1]->contains(search);
        std::cout << "Результат: " << (res ? "TRUE" : "FALSE") << "\n";
    }
    else if (type == 3) {
        if (doubleObjects.empty()) {
            std::cout << "Нет объектов\n";
            return;
        }
        size_t idx;
        std::cout << "Номер объекта (1-" << doubleObjects.size() << "): ";
        std::cin >> idx;
        if (idx < 1 || idx > doubleObjects.size()) {
            std::cout << "Неверно\n";
            return;
        }
        double search;
        std::cout << "Что ищем: ";
        std::cin >> search;
        bool res = doubleObjects[idx-1]->contains(search);
        std::cout << "Результат: " << (res ? "TRUE" : "FALSE") << "\n";
    }
}

void mostPopular() {
    printSeparator();
    std::cout << "Какой тип?\n";
    std::cout << "1. String/List<string>/StringList\n";
    std::cout << "2. List<int>\n";
    std::cout << "3. List<double>\n";
    std::cout << "Выбор: ";
    
    int typeChoice;
    std::cin >> typeChoice;
    
    if (typeChoice == 1) {
        if (stringObjects.empty()) {
            std::cout << "Нет объектов\n";
            return;
        }
        size_t idx;
        std::cout << "Номер объекта (1-" << stringObjects.size() << "): ";
        std::cin >> idx;
        if (idx < 1 || idx > stringObjects.size()) {
            std::cout << "Неверно\n";
            return;
        }
    }
    else if (typeChoice == 2) {
        if (intObjects.empty()) {
            std::cout << "Нет объектов\n";
            return;
        }
        size_t idx;
        std::cout << "Номер объекта (1-" << intObjects.size() << "): ";
        std::cin >> idx;
        if (idx < 1 || idx > intObjects.size()) {
            std::cout << "Неверно\n";
            return;
        }
    }
    else if (typeChoice == 3) {
        if (doubleObjects.empty()) {
            std::cout << "Нет объектов\n";
            return;
        }
        size_t idx;
        std::cout << "Номер объекта (1-" << doubleObjects.size() << "): ";
        std::cin >> idx;
        if (idx < 1 || idx > doubleObjects.size()) {
            std::cout << "Неверно\n";
            return;
        }
    }
}

void runTest() {
    printSeparator();
    std::cout << "\n=== ТЕСТ ===\n";
    
    // ========== STRING ==========
    String s1("hello world");
    std::cout << "\n[String] \"";
    s1.print();
    std::cout << "\"";
    std::cout << "\n  contains(\"world\"): " << (s1.contains("world") ? "TRUE" : "FALSE");
    std::cout << "\n  contains(\"hello\"): " << (s1.contains("hello") ? "TRUE" : "FALSE");
    std::cout << "\n  contains(\"xyz\"):   " << (s1.contains("xyz") ? "TRUE" : "FALSE");
    std::cout << "\n  contains(\"lo wo\"): " << (s1.contains("lo wo") ? "TRUE" : "FALSE");
    
    // ========== LIST<STRING> ==========
    List<std::string> l1;
    l1.push_back("test");
    l1.push_back("example");
    l1.push_back("test");
    l1.push_back("hello world");
    
    std::cout << "\n\n[List<string>] содержимое: ";
    l1.print();
    std::cout << "  contains(\"test\"):       " << (l1.contains("test") ? "TRUE" : "FALSE");
    std::cout << "\n  contains(\"example\"):    " << (l1.contains("example") ? "TRUE" : "FALSE");
    std::cout << "\n  contains(\"hello world\"): " << (l1.contains("hello world") ? "TRUE" : "FALSE");
    std::cout << "\n  contains(\"xyz\"):        " << (l1.contains("xyz") ? "TRUE" : "FALSE");
    
    // ========== STRINGLIST ==========
    StringList sl1("start");
    sl1.push_back("another test");
    sl1.push_back("hello world");
    sl1.push_back("example");
    sl1.push_back("hello world");
    
    std::cout << "\n\n[StringList] содержимое списка: ";
    sl1.print();
    std::cout << "  contains(\"test\"):   " << (sl1.contains("test") ? "TRUE" : "FALSE");
    std::cout << "\n  contains(\"start\"):  " << (sl1.contains("start") ? "TRUE" : "FALSE");
    std::cout << "\n  contains(\"world\"):  " << (sl1.contains("world") ? "TRUE" : "FALSE");
    std::cout << "\n  contains(\"examp\"):  " << (sl1.contains("examp") ? "TRUE" : "FALSE");
    std::cout << "\n  contains(\"xyz\"):    " << (sl1.contains("xyz") ? "TRUE" : "FALSE");
    
    // ========== LIST<INT> ==========
    List<int> l2;
    l2.push_back(42);
    l2.push_back(100);
    l2.push_back(42);
    l2.push_back(7);
    l2.push_back(42);
    
    std::cout << "\n\n[List<int>] содержимое: ";
    l2.print();
    std::cout << "  contains(42):  " << (l2.contains(42) ? "TRUE" : "FALSE");
    std::cout << "\n  contains(100): " << (l2.contains(100) ? "TRUE" : "FALSE");
    std::cout << "\n  contains(7):   " << (l2.contains(7) ? "TRUE" : "FALSE");
    std::cout << "\n  contains(999): " << (l2.contains(999) ? "TRUE" : "FALSE");
    
    // ========== LIST<DOUBLE> ==========
    List<double> l3;
    l3.push_back(3.14);
    l3.push_back(2.71);
    l3.push_back(3.14);
    
    std::cout << "\n\n[List<double>] содержимое: ";
    l3.print();
    std::cout << "  contains(3.14): " << (l3.contains(3.14) ? "TRUE" : "FALSE");
    std::cout << "\n  contains(2.71): " << (l3.contains(2.71) ? "TRUE" : "FALSE");
    std::cout << "\n  contains(1.0):  " << (l3.contains(1.0) ? "TRUE" : "FALSE");
    
    std::cout << "\n";
    printSeparator();
}

// ============= MAIN =============

int main(int argc, char* argv[]) {
    if (argc > 1 && std::string(argv[1]) == "test") {
        runTest();
        return 0;
    }
    
    while (true) {
        showMenu();
        
        int choice;
        std::cin >> choice;
        
        if (choice == 0) break;
        
        if (choice == 1) {
            char buffer[1000];
            std::cout << "Введите строку: ";
            std::cin.ignore();
            std::cin.getline(buffer, 1000);
            stringObjects.push_back(new String(buffer));
            std::cout << "String создан\n";
        }
        else if (choice == 2) {
            stringObjects.push_back(new List<std::string>());
            std::cout << "List<string> создан\n";
        }
        else if (choice == 3) {
            char buffer[1000];
            std::cout << "Введите строку: ";
            std::cin.ignore();
            std::cin.getline(buffer, 1000);
            stringObjects.push_back(new StringList(buffer));
            std::cout << "StringList создан\n";
        }
        else if (choice == 4) {
            intObjects.push_back(new List<int>());
            std::cout << "List<int> создан\n";
        }
        else if (choice == 5) {
            doubleObjects.push_back(new List<double>());
            std::cout << "List<double> создан\n";
        }
        else if (choice == 6) {
            showAll();
        }
        else if (choice == 7) {
            addElement();
        }
        else if (choice == 8) {
            checkContains();
        }
        else if (choice == 9) {
            runTest();
        }
        else if (choice == 10) {
            mostPopular();
        }
    }
    
    for (auto* obj : stringObjects) delete obj;
    for (auto* obj : intObjects) delete obj;
    for (auto* obj : doubleObjects) delete obj;
    
    return 0;
}