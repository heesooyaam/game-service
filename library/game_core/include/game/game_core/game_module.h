#pragma once

#include <string>

namespace game::game_core {

enum class EGameMode {
    TurnBased,
    Realtime
};

class IGameModule {
public:
    virtual ~IGameModule() = default;

    virtual std::string game_type() const = 0;
    virtual EGameMode mode() const = 0;
};

} // namespace game::game_core
