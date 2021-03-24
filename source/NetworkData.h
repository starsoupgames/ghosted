#pragma once

#ifndef __NETWORK_DATA_H__
#define __NETWORK_DATA_H__
#include <cugl/cugl.h>
#include "GameEntities/Player.h"

/** Data to be sent over the network */
class NetworkData {
private:
    uint8_t _id;

    Player _player;

public:
    NetworkData() { };

    ~NetworkData() { };

    void init();

    uint8_t getID() {
        return _id;
    }

    void setID(uint8_t id) {
        _id = id;
    }

    void setPlayer(Player player) {
        _player = player;
    }

};
#endif /** __NETWORK_DATA_H__ */