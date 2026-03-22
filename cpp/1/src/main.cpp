#include "ui.hpp"

int main() {
    try {
        menu();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return 1;
    }

    return 0;

}
