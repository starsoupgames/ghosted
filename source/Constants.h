#pragma once

#ifndef __CONSTANTS_H__
#define __CONSTANTS_H__

#include <cstdint>

using namespace cugl;

/**
 * Namespace for constants that should be globally accessible
 */
namespace constants {
    constexpr unsigned SCENE_WIDTH = 1024;

    constexpr uint8_t NETWORK_TICKS = 5;

    const Vec2 FLASHLIGHT_OFFSET (0, -50);

    const Vec2 ROOM_DIMENSIONS (375, 375);

    const Vec2 ROOM_CENTER (187.5, 187.5);

    const Vec2 GHOST_SHADOW_OFFSET (10, -15);

    const Vec2 PAL_SHADOW_OFFSET (2, -20);

    const Vec2 PAL_SMOKE_OFFSET(-10, 0);


    // Make sure to change values in lightShader.frag if you change these here
    /**maximum possible rooms generated in a map**/
    constexpr uint8_t MAX_ROOMS = 16;

    /**maximum possible pals in a game**/
    constexpr uint8_t MAX_PALS = 3;

    /** Game modes */
    enum class GameMode {
        None,
        Loading,
        Start,
        CreateGame,
        JoinGame,
        Lobby,
        Game,
        Win
    };

    /** Node priority levels */
    constexpr unsigned PRIORITY_RANGE = 100;
    enum Priority {
        Background = 0,
        Room = 100, // 100-199
        RoomEntity = 200, // 200-299
        Player = 300, // 300-399
    };

    /** Status of the match */
    enum MatchStatus {
        None = 0,
        Waiting = 1, // connected, waiting for players
        InProgress = 2, // game is ongoing
        Paused = 3, // player disconnected, game paused
        Ended = 4 // game ended
    };
}

#endif // __CONSTANTS_H__
