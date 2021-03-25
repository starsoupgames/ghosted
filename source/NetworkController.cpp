#include "NetworkController.h"

using namespace cugl;
using namespace std;

void NetworkController::init(const shared_ptr<NetworkData>& data) {
    _data = data;
}

void NetworkController::connect() {
    if (_connected) return;
    const auto config = CUNetworkConnection::ConnectionConfig(SERVER_ADDRESS, SERVER_PORT, MAX_PLAYERS, API_VERSION);
    _connection = make_shared<CUNetworkConnection>(config);
    _connected = true;
}

void NetworkController::connect(string roomID) {
    if (_connected) return;
    const auto config = CUNetworkConnection::ConnectionConfig(SERVER_ADDRESS, SERVER_PORT, MAX_PLAYERS, API_VERSION);
    _connection = make_shared<CUNetworkConnection>(config, roomID);
    _roomID = roomID;
    _connected = true;
}

void NetworkController::update(float timestep) {
    if (!_connected) return;

    if (_connection->getNumPlayers() < _connection->getTotalPlayers()) {
        CULog("Player exited the game.");
    }

    // update room id
    if (_roomID != _connection->getRoomID()) {
        _roomID = _connection->getRoomID();
        // TODO temp
        /*
        const auto config = CUNetworkConnection::ConnectionConfig(SERVER_ADDRESS, SERVER_PORT, MAX_PLAYERS, API_VERSION);
        c2 = make_shared<CUNetworkConnection>(config, _roomID);
        */
    }

    // update player id
    if (_connection->getPlayerID()) _data->setID(_connection->getPlayerID().value());

    // send data
    if (_connected && ++_tick >= NETWORK_TICKS) {
        sendData();
        _tick = 0;
    }
    // receive data
    if (_connection == nullptr) {
        CULog("Null network connection.");
        return;
    }
    _connection->receive([this](const vector<uint8_t>& msg) {
        receiveData(msg);
        });

    // TODO temp
    /*
    if (c2 == nullptr) {
        CULog("Null network connection C2.");
        return;
    }
    c2->receive([this](const vector<uint8_t>& msg) {
        receiveData(msg);
        });
    */
}

bool NetworkController::sendData() {
    if (_data->getID() < 0) {
        CULog("No id attached to network data.");
        return false;
    }
    _connection->send(_data->serializeData());
    return true;
}

bool NetworkController::receiveData(const vector<uint8_t>& msg) {
    if (msg.empty()) {
        return false;
    }
    _data->unserializeData(msg);
    return true;
}