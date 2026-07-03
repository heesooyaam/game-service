#include <game/common/version.h>

#include <iostream>

int main() {
    std::cout << "storage_service " << game::common::version() << '\n';
    return 0;
}
