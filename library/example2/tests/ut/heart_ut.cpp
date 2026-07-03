#include <game/example2/heart.h>

#include <cassert>

int main() {
    assert(game::example2::sum(1, 2) == 3);
    assert(game::example2::sum(1, 3) == 4);
    assert(game::example2::sum(67, 0) == 67);

    return 0;
}
