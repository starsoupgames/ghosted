#pragma once

#ifndef __NETWORK_DATA_H__
#define __NETWORK_DATA_H__
#include <cugl/cugl.h>
#include "Constants.h"
#include "GameEntities/Player.h"

/** The precision to multiply floating point numbers by */
constexpr float FLOAT_PRECISION = 100.0f;

using namespace std;

/** Metadata */
struct NetworkMetadata {
    uint8_t status;
    int id; // Network data id

    NetworkMetadata(uint8_t status, int id) {
        this->status = status;
        this->id = id;
    }
};

/** Data used for player interpolation */
struct InterpolationData {
    unsigned ticksSinceReceived;
    Vec2 oldPosition;
    Vec2 newPosition;

    InterpolationData(Vec2 pos) {
        this->ticksSinceReceived = 0;
        this->oldPosition = pos;
        this->newPosition = pos;
    }
};

/** Player data */
struct PlayerData {
    int id; // Player id
    shared_ptr<Player> player; // NetworkData does not own this player
    shared_ptr<InterpolationData> interpolationData;

    PlayerData(int id, shared_ptr<Player> player) {
        this->id = id;
        this->player = player;
        this->interpolationData = make_shared<InterpolationData>(player->getLoc());
    }
    ~PlayerData() { player = nullptr; interpolationData = nullptr; };
};

/** Handles network data */
class NetworkData {
private:
    /** Player id */
    int _id;

    /** Player name */
    string _name;

    /** Host id */
    int _hostID; // TODO handle change of host

    /** Match status */
    uint8_t _status;

    /** Player data vector */
    vector<shared_ptr<PlayerData>> _players;
    /** Interpolation data vector */
    vector<shared_ptr<InterpolationData>> _interpolationData;

    /** Functions to convert data types to byte vectors */
    void encodeByte(uint8_t b, vector<uint8_t>& out); // 1 byte
    uint8_t decodeByte(const vector<uint8_t>& bytes);

    // adapted from https://os.mbed.com/forum/helloworld/topic/2053/?page=1#comment-54126
    void encodeInt(int i, vector<uint8_t>& out); // 4 bytes
    int decodeInt(const vector<uint8_t>& bytes);
    
    void encodeFloat(float f, vector<uint8_t>& out); // 4 bytes
    float decodeFloat(const vector<uint8_t>& bytes);

    void encodeVector(const Vec2& v, vector<uint8_t>& out); // 8 bytes
    Vec2 decodeVector(const vector<uint8_t>& bytes);

    /** Convert metadata */
    vector<uint8_t> convertMetadata();
    shared_ptr<NetworkMetadata> interpretMetadata(const vector<uint8_t>& metadata);

    /** Convert and interpret player data */
    vector<uint8_t> convertPlayerData();
    void interpretPlayerData(const int id, const vector<uint8_t>& playerData);

    /** Convert and interpret map data */
    vector<uint8_t> convertMapData();
    void interpretMapData(const vector<uint8_t>& mapData);

public:
    NetworkData() : _id(-1), _name("Player"), _hostID(0), _status(constants::MatchStatus::None) { };

    ~NetworkData() { };

    /** @return main player */
    shared_ptr<PlayerData> getPlayer();

    /** @return player with id */
    shared_ptr<PlayerData> getPlayer(int id);

    /** @return player id */
    uint8_t getID() {
        return _id;
    }

    /** Set player id. */
    void setID(int id) {
        _id = id;
    }

    /** Set players as player data vector */
    void setPlayers(const vector<shared_ptr<Player>>& players) {
        CUAssertLog(players.size() == 4, "Players vector is not size 4.");
        _players = vector<shared_ptr<PlayerData>>(4, nullptr);
        for (int i = 0; i < 4; ++i) {
            if (players[i] != nullptr) {
                _players[i] = make_shared<PlayerData>(i, players[i]);
            }
        }
    }

    /** @return match status */
    uint8_t getStatus() {
        return _status;
    }

    /** Set match status */
    void setStatus(const uint8_t status) {
        _status = status;
    }

    /** Serialize game data */
    vector<uint8_t> serializeData();

    /** Unserialize game data */
    void unserializeData(const vector<uint8_t>& msg);

    /** Interpolate player data using interpolation data */
    void interpolatePlayerData();
};
#endif /** __NETWORK_DATA_H__ */