#include "NetworkController.h"

using namespace cugl;
using namespace std;

void NetworkController::connect() {
    if (_connection != nullptr || _connected) return;
    const auto config = CUNetworkConnection::ConnectionConfig(SERVER_ADDRESS, SERVER_PORT, MAX_PLAYERS, API_VERSION);
    _connection = make_shared<CUNetworkConnection>(config);
    _host = true;
}

void NetworkController::connect(string roomID) {
    if (_connection != nullptr || _connected) return;
    const auto config = CUNetworkConnection::ConnectionConfig(SERVER_ADDRESS, SERVER_PORT, MAX_PLAYERS, API_VERSION);
    _connection = make_shared<CUNetworkConnection>(config, roomID);
    _roomID = roomID;
    _host = false;
}

void NetworkController::disconnect() {
    _connection = nullptr;
    _connected = false;
}

void NetworkController::updateStatus() {
    auto status = getStatus();
    if (_status == status) return;

    // old status
    switch (_status) {
    case CUNetworkConnection::NetStatus::Disconnected:
        break;
    case CUNetworkConnection::NetStatus::Pending:
        break;
    case CUNetworkConnection::NetStatus::Connected:
        _connected = false;
        break;
    case CUNetworkConnection::NetStatus::Reconnecting:
        break;
    case CUNetworkConnection::NetStatus::RoomNotFound:
        break;
    case CUNetworkConnection::NetStatus::ApiMismatch:
        break;
    case CUNetworkConnection::NetStatus::GenericError:
        break;
    }

    // new status
    switch (status) {
    case CUNetworkConnection::NetStatus::Disconnected:
        break;
    case CUNetworkConnection::NetStatus::Pending:
        break;
    case CUNetworkConnection::NetStatus::Connected:
        _connected = true;
        _roomID = _connection->getRoomID();
        if (_data != nullptr) _data->setID(_connection->getPlayerID().value());
        break;
    case CUNetworkConnection::NetStatus::Reconnecting:
        break;
    case CUNetworkConnection::NetStatus::RoomNotFound:
        break;
    case CUNetworkConnection::NetStatus::ApiMismatch:
        break;
    case CUNetworkConnection::NetStatus::GenericError:
        break;
    }

    _status = status;
}

void NetworkController::update(float timestep) {
    if (_connection == nullptr) return;

    // receive data
    _connection->receive([this](const vector<uint8_t>& msg) {
        receiveData(msg);
        });

    // update status
    updateStatus();

    // handle dropped player
    if (_connection->getNumPlayers() < _connection->getTotalPlayers()) {
        // CULog("Player exited the game.");
    }

    // interpolate player data
    if (_data != nullptr) {
        if (_connected) {
            _data->interpolatePlayerData();
        }
    }

    // send data
    if (_connected && ++_tick >= constants::NETWORK_TICKS) {
        sendData();
        _tick = 0;
    }
}

bool NetworkController::sendData() {
    if (_data == nullptr) {
        // CULog("No data attached.");
        return false;
    }
    _connection->send(_data->serializeData());
    return true;
}

bool NetworkController::receiveData(const vector<uint8_t>& msg) {
    if (msg.empty()) {
        return false;
    }
    if (_data == nullptr) {
        // CULog("No data attached.");
        return false;
    }
    _data->unserializeData(msg);
    return true;
}