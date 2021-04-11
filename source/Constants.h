#pragma once

#ifndef __CONSTANTS_H__
#define __CONSTANTS_H__

#include <cstdint>

/**
 * Namespace for constants that should be globally accessible
 */
namespace constants {
    constexpr unsigned SCENE_WIDTH = 1024;
    constexpr uint8_t NETWORK_TICKS = 5;

    /** Game modes */
    enum GameMode {
        None = 0,
        Loading = 1,
        Start = 10,
        CreateGame = 11,
        JoinGame = 12,
        Lobby = 20,
        Game = 21
    };

    /** Node priority levels */
    constexpr unsigned PRIORITY_RANGE = 100;
    enum Priority {
        Background = 0,
        Room = 100, // 100-199
        RoomEntity = 200, // 200-299
        Player = 300, // 300-399
    };
}

#endif // __CONSTANTS_H__
