#pragma once

#include <game/common/ids.h>

#include <optional>

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
