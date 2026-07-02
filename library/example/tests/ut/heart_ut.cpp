#include <cassert>
#include <string>

#include <game/example/heart.h>

int main() {
    assert(game::example::heart() == "<3");
    assert(game::example::heart_message("") == "<3");
    assert(game::example::heart_message("json") == "<3 json");

    return 0;
}
