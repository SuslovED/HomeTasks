#include "Parser.h"
#include <fstream>
#include <iostream>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " source.m" << std::endl;
        return 1;
    }
    std::ifstream file(argv[1]);
    if (!file) {
        std::cerr << "Cannot open file" << std::endl;
        return 1;
    }
    Parser parser(file);
    parser.analyze();
    return 0;
}