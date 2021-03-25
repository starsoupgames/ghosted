#pragma once

#ifndef __NETWORK_DATA_H__
#define __NETWORK_DATA_H__
#include <cugl/cugl.h>
#include "GameEntities/Player.h"

/** The precision to multiply floating point numbers by */
constexpr float FLOAT_PRECISION = 100.0f;

using namespace std;

/** Data to be sent over the network */
class NetworkData {
private:
    uint8_t _id;

    shared_ptr<Player> _player; // NetworkData does not own this player
    shared_ptr<Player> _otherPlayer; // NetworkData does not own this player

    void encodeByte(uint8_t b, vector<uint8_t>& out); // 1 byte
    uint8_t decodeByte(const vector<uint8_t>& bytes);

    // adapted from https://os.mbed.com/forum/helloworld/topic/2053/?page=1#comment-54126
    void encodeFloat(float f, vector<uint8_t>& out); // 4 bytes
    float decodeFloat(const vector<uint8_t>& bytes);

    void encodeVector(Vec2 v, vector<uint8_t>& out); // 8 bytes
    Vec2 decodeVector(const vector<uint8_t>& bytes);

    /** Split byte vector at certain points */
    vector<vector<uint8_t>> split(const vector<uint8_t>& bytes, const vector<uint8_t>& indices);

    /** Metadata */
    vector<uint8_t> convertMetadata();

    /** Player data */
    vector<uint8_t> convertPlayerData();
    void interpretPlayerData(const vector<uint8_t>& metadata, const vector<uint8_t>& playerData);

    /** Map data */
    vector<uint8_t> convertMapData();
    void interpretMapData(const vector<uint8_t>& mapData);

public:
    NetworkData() : _id(-1) { };

    ~NetworkData() { };

    bool init();

    static shared_ptr<NetworkData> alloc() {
        shared_ptr<NetworkData> result = make_shared<NetworkData>();
        return (result->init() ? result : nullptr);
    }

    uint8_t getID() {
        return _id;
    }

    void setID(uint8_t id) {
        _id = id;
    }

    void setPlayer(const shared_ptr<Player>& player) {
        _player = player;
    }

    // temp
    void setOtherPlayer(const shared_ptr<Player>& otherPlayer) {
        _otherPlayer = otherPlayer;
    }

    vector<uint8_t> serializeData();

    void unserializeData(const vector<uint8_t>& msg);

};
#endif /** __NETWORK_DATA_H__ */