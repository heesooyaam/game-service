#include <iostream>

#include <game/common/version.h>
int main() {
    std::cout << "game_service " << game::common::version() << '\n';
    return 0;
}
