#pragma once

#ifndef __NETWORK_CONTROLLER_H__
#define __NETWORK_CONTROLLER_H__
#include <cugl/cugl.h>
#include "Constants.h"
#include "NetworkData.h"

/** Constants for connection config */
constexpr char* SERVER_ADDRESS = "34.86.11.83";
constexpr uint16_t SERVER_PORT = 61111;
constexpr uint32_t MAX_PLAYERS = 4;
constexpr uint8_t API_VERSION = 0;

using namespace std;
using namespace cugl;

/** Handles networking */
class NetworkController {
private:
    /** Tick */
    unsigned _tick;

    /** Network data */
    shared_ptr<NetworkData> _data; // NetworkController does not own this

    /** Network connection */
    shared_ptr<CUNetworkConnection> _connection;

    /** Connection status */
    CUNetworkConnection::NetStatus _status;

    /** Whether the player is connected */
    bool _connected;

    /** Whether the player is the host */
    bool _host;

    /** Connection room id */
    string _roomID;

    /** Update connection status */
    void updateStatus();

    /** Send serialized data over network */
    bool sendData();

    /** Receive and interpret data from network */
    bool receiveData(const vector<uint8_t>& msg);

public:
    NetworkController() : _tick(0), _status(CUNetworkConnection::NetStatus::Disconnected), _connected(false), _host(true), _roomID("") { };

    ~NetworkController() { dispose(); };

    /** Attach network data */
    void attachData(const shared_ptr<NetworkData>& data) {
        _data = data;
    }

    /** @return network data */
    shared_ptr<NetworkData> getData() {
        return _data;
    };

    /** Start the game */
    void startGame() {
        if (_host && _connection != nullptr) _connection->startGame();
        if (_data != nullptr) _data->setStatus(constants::MatchStatus::InProgress);
    }

    /** Connect to network as host */
    void connect();

    /** Connect to network with roomID */
    void connect(string roomID);

    /** Disconnect from network */
    void disconnect();

    /** @return if the player is connected */
    bool isConnected() {
        return _connected;
    }

    /** @return if the player is the host */
    bool isHost() {
        return _host;
    }

    /** @return the connection status */
    CUNetworkConnection::NetStatus getStatus() {
        return _connection == nullptr ? CUNetworkConnection::NetStatus::Disconnected : _connection->getStatus();
    }

    /** @return the connection room id */
    string getRoomID() {
        return _roomID;
    }

    /** @return the number of connected players */
    uint8_t getNumPlayers() {
        return _connection == nullptr ? 0 : _connection->getNumPlayers();
    }

    /** Set the players */
    void setPlayers(const vector<shared_ptr<Player>>& players) {
        if (_data != nullptr) {
            _data->setPlayers(players);
        }
    }

    void update(float timestep);

    void dispose() {
        disconnect();
        _data = nullptr;
    };

};
#endif /** __NETWORK_CONTROLLER_H__ */