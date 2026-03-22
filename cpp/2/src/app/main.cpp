#include <iostream>
#include "StudentWork.hpp"

int main() {
    StudentWork work;

    try {
        std::cout << "Input data of Student Work:\n";
        std::cin >> work;
        std::cout << "Data entered successfully!\n";

    } catch (const std::exception& e) {
        std::cerr << "ERROR: " << e.what() << std::endl;
        return 1;
    }

    std::cout << work << std::endl;


    return 0;
}
