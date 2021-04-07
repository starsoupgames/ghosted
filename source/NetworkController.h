#pragma once

#ifndef __NETWORK_CONTROLLER_H__
#define __NETWORK_CONTROLLER_H__
#include <cugl/cugl.h>
#include "Constants.h"
#include "NetworkData.h"

constexpr char* SERVER_ADDRESS = "34.74.68.73";
constexpr uint16_t SERVER_PORT = 61111;
constexpr uint32_t MAX_PLAYERS = 4;
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
    bool _connected;
    string _roomID;

    bool sendData();

    bool receiveData(const vector<uint8_t>& msg);

public:
    NetworkController() : _tick(0), _connected(false), _roomID("") { };

    ~NetworkController() { };

    string getRoomID() {
        return _roomID;
    }

    void init(const shared_ptr<NetworkData>& data);

    void connect();

    void connect(string roomID);

    void update(float timestep);

    void dispose() {
        _connection = nullptr;
    };

};
#endif /** __NETWORK_CONTROLLER_H__ */