#pragma once

#include <optional>

#include <game/common/ids.h>

namespace game::game_core {

enum class EGameStatus {
    InProgress,
    Draw,
    Finished
};

struct TGameResult {
    EGameStatus status = EGameStatus::InProgress;
    std::optional<common::TPlayerId> winner_player_id;
};

} // namespace game::game_core
