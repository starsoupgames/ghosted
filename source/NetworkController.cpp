#include "NetworkController.h"

using namespace cugl;
using namespace std;

void NetworkController::init() {
}

void NetworkController::connect() {
    if (connected) return;
    const auto config = CUNetworkConnection::ConnectionConfig(SERVER_ADDRESS, SERVER_PORT, MAX_PLAYERS, API_VERSION);
    connection = make_shared<CUNetworkConnection>(config);
    connected = true;
}

void NetworkController::connect(string roomID) {
    if (connected) return;
    const auto config = CUNetworkConnection::ConnectionConfig(SERVER_ADDRESS, SERVER_PORT, MAX_PLAYERS, API_VERSION);
    connection = make_shared<CUNetworkConnection>(config, roomID);
    connected = true;
}

void NetworkController::update(float timestep) {
    if (!connected) return;

    // update room id
    if (roomID != connection->getRoomID()) {
        roomID = connection->getRoomID();
        // temp benshen
        // const auto config = CUNetworkConnection::ConnectionConfig(SERVER_ADDRESS, SERVER_PORT, MAX_PLAYERS, API_VERSION);
        // c2 = make_shared<CUNetworkConnection>(config, roomID);
    }

    // update player id
    if (playerID < 0 && connection->getPlayerID()) playerID = connection->getPlayerID().value();

    // send data
    if (connected && ++tick >= NETWORK_TICKS) {
        if (!sendData()) CULog("Error sending data.");
        tick = 0;
    }
    // receive data
    if (connection == nullptr) {
        CULog("Null network connection.");
        return;
    }
    connection->receive([this](const std::vector<uint8_t>& msg) {
        receiveData(msg);
        });

    // temp benshen
    /*
    if (c2 == nullptr) {
        CULog("Null network connection.");
        return;
    }
    c2->receive([this](const std::vector<uint8_t>& msg) {
        receiveData(msg);
        });
    */
}

bool NetworkController::sendData() {
    vector<uint8_t> msg;
    char c = (char) rand() % 26 + 'a';
    // CULog("sending %c", c);
    msg.push_back(c);
    connection->send(msg);
    return true;
}

bool NetworkController::receiveData(const std::vector<uint8_t>& msg) {
    if (msg.empty()) {
        return false;
    }
    vector<uint8_t> received;
    for (uint8_t m : msg) {
        received.push_back(m);
    }
    for (uint8_t r : received) {
        // CULog("received %c", r);
    }
    return true;
}