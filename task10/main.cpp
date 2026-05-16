#include "Parser.h"
#include "Executor.h"
#include <fstream>
#include <iostream>

int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cerr << "Usage: " << argv[0] << " source.m [exec]" << std::endl;
        return 1;
    }
    std::ifstream file(argv[1]);
    if (!file) {
        std::cerr << "Cannot open file" << std::endl;
        return 1;
    }

    Parser parser(file);
    parser.analyze();

    const auto& poliz = parser.get_poliz();
    if (poliz.empty()) {
        // ошибка анализа – программа не сгенерирована
        return 1;
    }

    // Если анализ успешен, в конце poliz есть CMD_HALT
    if (argc == 3 && std::string(argv[2]) == "exec") {
        Executor executor(poliz, parser.get_TID());
        executor.run();
    }

    return 0;
}