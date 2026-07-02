#pragma once

#include <string>
#include <vector>

#include <game/common/ids.h>
#include <game/game_core/game_module.h>
#include <game/game_core/game_result.h>
#include <game/json/json_value.h>

namespace game::game_core {

struct TMoveValidationResult {
    bool ok = false;
    std::string error_code;
    std::string error_message;
};

class ITurnBasedGame : public IGameModule {
public:
    EGameMode mode() const override {
        return EGameMode::TurnBased;
    }

    virtual json::TJsonValue create_initial_state(
        const std::vector<common::TPlayerId>& players
    ) const = 0;

    virtual TMoveValidationResult validate_move(
        const json::TJsonValue& state,
        common::TPlayerId player,
        const json::TJsonValue& move
    ) const = 0;

    virtual json::TJsonValue apply_move(
        const json::TJsonValue& state,
        common::TPlayerId player,
        const json::TJsonValue& move
    ) const = 0;

    virtual TGameResult get_result(const json::TJsonValue& state) const = 0;
};

} // namespace game::game_core
