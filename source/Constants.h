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

    /**
     * Game modes
     */
    enum GameMode {
        None = 0,
        Loading = 1,
        Start = 10,
        CreateGame = 11,
        JoinGame = 12,
        Game = 20
    };
}

#endif // __CONSTANTS_H__
