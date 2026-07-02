#include <game/example/heart.h>

namespace game::example {

std::string heart() {
    return "<3";
}

std::string heart_message(const std::string& name) {
    if (name.empty()) {
        return heart();
    }

    return heart() + " " + name;
}

} // namespace game::example
