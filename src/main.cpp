#include "billiard/core/Version.hpp"
#include <iostream>

// #include <SFML/Graphics.hpp>

int main() {
    std::cout << "Billiard skeleton structure ready. Version: " << billiard::core::version() << std::endl;
    std::cout << "Enable SFML later with -DBILLIARD_WITH_SFML=ON and implement adapters in platform/sfml/."
              << std::endl;
    return 0;
}