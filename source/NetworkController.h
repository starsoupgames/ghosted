#pragma once

#ifndef __NETWORK_CONTROLLER_H__
#define __NETWORK_CONTROLLER_H__
#include <cugl/cugl.h>

constexpr char* SERVER_ADDRESS = "34.74.68.73";
constexpr uint16_t SERVER_PORT = 61111;
constexpr uint32_t MAX_PLAYERS = 4;
constexpr uint8_t API_VERSION = 0;

// number of ticks between sending data
constexpr uint8_t NETWORK_TICKS = 60;

using namespace std;
using namespace cugl;

/** Handles networking */
class NetworkController {
private:
    unsigned tick;

    shared_ptr<CUNetworkConnection> connection;
    bool connected;
    string roomID;
    int playerID;

    // temp benshen
    shared_ptr<CUNetworkConnection> c2;

    bool sendData();

    bool receiveData(const std::vector<uint8_t>& msg);

public:
    NetworkController() : tick(0), connected(false), roomID(""), playerID(-1) { };

    ~NetworkController() { };

    shared_ptr<CUNetworkConnection> getConnection() {
        return connection;
    }

    void init();

    void connect();

    void connect(string roomID);

    void update(float timestep);

    void dispose() { connection = nullptr; 
    // temp benshen
    c2 = nullptr; };

};
#endif /** __NETWORK_CONTROLLER_H__ */