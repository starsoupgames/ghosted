#pragma once

#ifndef __NETWORK_DATA_H__
#define __NETWORK_DATA_H__
#include <cugl/cugl.h>
#include "Constants.h"
#include "NetworkUtils.h"
#include "GameMap.h"
#include "GameEntities/Player.h"
#include "GameEntities/Players/PlayerPal.h"
#include "GameEntities/Players/PlayerGhost.h"

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

/** Lobby Data */
struct LobbyData {
    vector<int> playerOrder;

    LobbyData(vector<int>& playerOrder) {
        this->playerOrder = playerOrder;
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

    PlayerData(int id, shared_ptr<Player>& player) {
        this->id = id;
        this->player = player;
        this->interpolationData = make_shared<InterpolationData>(player->getLoc());
    }
    ~PlayerData() { player = nullptr; interpolationData = nullptr; };
};

/** Map data */
struct MapData {
    shared_ptr<GameMap> map;
    bool generated;

    MapData(shared_ptr<GameMap>& map) {
        this->map = map;
        this->generated = false;
    }
    ~MapData() { map = nullptr; };
};

/** Win data */
struct WinData {
    constants::PlayerType winner;

    WinData() {
        this->winner = constants::PlayerType::Undefined;
    }
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

    /** Lobby data */
    shared_ptr<LobbyData> _lobbyData;

    /** Player data vector */
    vector<shared_ptr<PlayerData>> _players;

    /** Map data */
    shared_ptr<MapData> _mapData;

    /** Win data */
    shared_ptr<WinData> _winData;

    /** Convert metadata */
    vector<uint8_t> convertMetadata();
    shared_ptr<NetworkMetadata> interpretMetadata(const vector<uint8_t>& metadata);

    /** Convert and interpret lobby data */
    vector<uint8_t> convertLobbyData();
    void interpretLobbyData(const int id, const vector<uint8_t>& lobbyData);

    /** Convert and interpret map layout data */
    vector<uint8_t> convertMapLayoutData();
    void interpretMapLayoutData(const int id, const vector<uint8_t>& mapLayoutData);

    /** Convert and interpret player data */
    vector<uint8_t> convertPlayerData();
    void interpretPlayerData(const int id, const vector<uint8_t>& playerData);

    /** Convert and interpret map data */
    vector<uint8_t> convertMapData();
    void interpretMapData(const int id, const vector<uint8_t>& mapData);

    /** Convert and interpret win data */
    vector<uint8_t> convertWinData();
    void interpretWinData(const int id, const vector<uint8_t>& winData);

public:
    NetworkData() : _id(-1), _name("Player"), _hostID(0), _status(constants::MatchStatus::None) { };

    ~NetworkData() { dispose(); };

    void dispose() {
        _lobbyData = nullptr;
        _players.clear();
        _winData = nullptr;
        _mapData = nullptr;
    }

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

    /** @return players */
    vector<shared_ptr<Player>> getPlayers() {
        vector<shared_ptr<Player>> players;
        for (auto& p : _players) {
            players.push_back(p->player);
        }
        return players;
    }

    /** Set players as player data vector */
    void setPlayers(vector<shared_ptr<Player>> players) {
        CUAssertLog(players.size() == 4, "Players vector is not size 4.");
        _players = vector<shared_ptr<PlayerData>>(4, nullptr);
        for (int i = 0; i < 4; ++i) {
            if (players[i] != nullptr) {
                if (_lobbyData != nullptr) {
                    _players[i] = make_shared<PlayerData>(i, players[_lobbyData->playerOrder[i]]);
                }
                else {
                    _players[i] = make_shared<PlayerData>(i, players[i]);
                }
            }
        }
    }

    /** Get game map */
    shared_ptr<GameMap> getGameMap() {
        if (_mapData == nullptr) return nullptr;
        return _mapData->map;
    }

    /** Set game map */
    void setGameMap(shared_ptr<GameMap> gameMap) {
        _mapData = make_shared<MapData>(gameMap);
    }

    /** @return match status */
    uint8_t getStatus() {
        return _status;
    }

    /** Set match status */
    void setStatus(const uint8_t status) {
        _status = status;
    }

    /** @return winner */
    constants::PlayerType getWinner() {
        if (_winData == nullptr) return constants::PlayerType::Undefined;
        return _winData->winner;
    }

    /** Set winner */
    void setWinner(const constants::PlayerType winner) {
        if (_winData == nullptr) _winData = make_shared<WinData>();
        _winData->winner = winner;
    }

    /** Serialize game data */
    vector<uint8_t> serializeData();

    /** Unserialize game data */
    void unserializeData(const vector<uint8_t>& msg);

    /** Interpolate player data using interpolation data */
    void interpolatePlayerData();
};
#endif /** __NETWORK_DATA_H__ */