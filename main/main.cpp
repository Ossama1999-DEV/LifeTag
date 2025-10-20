#include <iostream>
#include "version.h"

int main() {
    std::cout << "=== Bienvenue dans LifeTag ===" << std::endl;
    std::cout << "Version : " << LifeTag::getVersion() << std::endl;
    return 0;
}
