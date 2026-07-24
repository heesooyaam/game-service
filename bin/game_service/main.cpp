#include <library/common/version.h>

#include <iostream>

int main() {
    std::cout << "game_service " << game::common::version() << '\n';
    return 0;
}
