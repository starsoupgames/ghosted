#pragma once

#ifndef __NETWORK_CONTROLLER_H__
#define __NETWORK_CONTROLLER_H__
#include <cugl/cugl.h>
#include "Constants.h"
#include "NetworkData.h"

constexpr char* SERVER_ADDRESS = "34.86.11.83";
constexpr uint16_t SERVER_PORT = 61111;
constexpr uint32_t MAX_PLAYERS = 2;
constexpr uint8_t API_VERSION = 0;

using namespace std;
using namespace cugl;

/** Handles networking */
class NetworkController {
private:
    unsigned _tick;

    // network controller does not own network data
    shared_ptr<NetworkData> _data;

    shared_ptr<CUNetworkConnection> _connection;
    CUNetworkConnection::NetStatus _status;

    bool _connected;
    bool _host;

    uint8_t _playerID;
    string _roomID;

    void updateStatus();

    bool sendData();

    bool receiveData(const vector<uint8_t>& msg);

public:
    NetworkController() : _tick(0), _status(CUNetworkConnection::NetStatus::Disconnected), _connected(false), _host(true), _playerID(0), _roomID("") { };

    ~NetworkController() { dispose(); };

    void attachData(const shared_ptr<NetworkData>& data);

    void startGame() {
        if (_host && _connection != nullptr) _connection->startGame();
    }

    void connect();

    void connect(string roomID);

    void disconnect();

    bool isConnected() {
        return _connected;
    }

    bool isHost() {
        return _host;
    }

    CUNetworkConnection::NetStatus getStatus() {
        return _connection == nullptr ? CUNetworkConnection::NetStatus::Disconnected : _connection->getStatus();
    }

    string getRoomID() {
        return _roomID;
    }

    uint8_t getNumPlayers() {
        return _connection == nullptr ? 0 : _connection->getNumPlayers();
    }

    void update(float timestep);

    void dispose() {
        _data = nullptr;
        _connection = nullptr;
    };

};
#endif /** __NETWORK_CONTROLLER_H__ */