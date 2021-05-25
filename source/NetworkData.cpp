#include "NetworkData.h"

using namespace std;
using namespace cugl;
using namespace NetworkUtils;

shared_ptr<PlayerData> NetworkData::getPlayer() {
    return getPlayer(_id);
};

shared_ptr<PlayerData> NetworkData::getPlayer(int id) {
    for (auto& p : _players) {
        if (p != nullptr && p->id == id) {
            return p;
        }
    }
    return nullptr;
};

vector<uint8_t> NetworkData::convertMetadata() {
    vector<uint8_t> result;
    if (_id < 0) {
        CULog("ID is not defined.");
        return result;
    }

    // match status
    encodeByte(_status, result);

    // player id
    encodeInt(_id, result);

    return result;
}

shared_ptr<NetworkMetadata> NetworkData::interpretMetadata(const vector<uint8_t>& metadata) {
    CUAssertLog(metadata.size() == 5, "interpretMetadata: Byte vector size is not equal to data type size.");
    vector<vector<uint8_t>> splitMetadata = split(metadata, { 1, 4 });
    int status = decodeByte(splitMetadata[0]);
    int id = decodeInt(splitMetadata[1]);
    return make_shared<NetworkMetadata>(status, id);
}

vector<uint8_t> NetworkData::convertLobbyData() {
    vector<uint8_t> result;

    if (_id < 0) {
        CULog("ID is not defined.");
        return result;
    }

    if (_id == _hostID) {
        if (_lobbyData == nullptr) {
            vector<int> playerOrder({ 3, 0, 1, 2 });
            // shuffle(playerOrder.begin(), playerOrder.end(), random_device());
            _lobbyData = make_shared<LobbyData>(playerOrder);
        }

        for (int i : _lobbyData->playerOrder) {
            encodeInt(i, result);
        }
    }

    return result;
}

void NetworkData::interpretLobbyData(const int id, const vector<uint8_t>& msg) {
    vector<vector<uint8_t>> splitMsg = split(msg, { 16 });
    if (splitMsg.empty()) return;
    if (id == _hostID && _lobbyData == nullptr) {
        vector<vector<uint8_t>> splitPlayerOrder = split(splitMsg[0], { 4, 4, 4, 4 });
        vector<int> playerOrder(4);
        for (int i = 0; i < 4; ++i) {
            playerOrder[i] = decodeInt(splitPlayerOrder[i]);
        }
        _lobbyData = make_shared<LobbyData>(playerOrder);
    }
}

vector<uint8_t> NetworkData::convertMapLayoutData() {
    vector<uint8_t> result;

    if (_id < 0) {
        CULog("ID is not defined.");
        return result;
    }

    if (_id == _hostID) {
        if (_mapData == nullptr || _mapData->map == nullptr) {
            return result;
        }

        auto data = _mapData->map->makeNetworkMap();
        encodeVector(data->startRank, result); // Vec2
        encodeVector(data->endRank, result); // Vec2

        /*
        encodeVec2List(data->batteries, result); // vector<Vec2>

        encodeInt((int)data->rooms.size(), result); // int
        for (auto& roomData : data->rooms) {
            encodeInt(roomData->layout, result); // int
            encodeVector(roomData->rank, result); // Vec2
            encodeBoolList(roomData->doors, result); // vector<bool>
        }
        */
    }

    return result;
}

void NetworkData::interpretMapLayoutData(const int id, const vector<uint8_t>& msg) {
    if (msg.empty()) return;

    if (_mapData == nullptr || _mapData->map == nullptr) return;

    vector<vector<uint8_t>> splitMsg;
    if (id == _hostID) {
        /*
        if (_mapData->generated) return;

        splitMsg = split(msg, { 8, 8 });
        auto startRank = decodeVector(splitMsg[0]);
        auto endRank = decodeVector(splitMsg[1]);

        splitMsg = split(splitMsg[2], { 4 });
        unsigned batteriesLength = decodeInt(splitMsg[0]);
        vector<Vec2> batteries;
        if (batteriesLength > 0) {
            splitMsg = split(splitMsg[1], { batteriesLength * 8 });
            if (splitMsg.)
            batteries = decodeVec2List(batteriesLength, splitMsg[0]);
        }

        splitMsg = split(splitMsg[1], { 4 });
        unsigned roomsLength = decodeInt(splitMsg[0]);
        vector<shared_ptr<RoomNetworkdata>> rooms;
        for (unsigned i = 0; i < roomsLength; i++) {
            splitMsg = split(splitMsg[1], { 4, 8 });
            auto layout = decodeInt(splitMsg[0]);
            auto rank = decodeVector(splitMsg[1]);

            splitMsg = split(splitMsg[2], { 4 });
            unsigned doorsLength = decodeInt(splitMsg[0]);
            vector<bool> doors;
            if (doorsLength > 0) {
                splitMsg = split(splitMsg[1], { batteriesLength * 1 });
                doors = decodeBoolList(doorsLength, splitMsg[0]);
            }

            rooms.push_back(make_shared<RoomNetworkdata>(doors, rank, layout));
        }

        auto data = make_shared<MapNetworkdata>(rooms, batteries, startRank, endRank);
        _mapData->map->readNetworkMap(data);
        */
    }
}

vector<uint8_t> NetworkData::convertPlayerData() {
    vector<uint8_t> result;
    if (_id < 0) {
        CULog("Player id is not defined.");
        return result;
    }

    shared_ptr<PlayerData> playerData = getPlayer();

    if (playerData == nullptr || playerData->player == nullptr) {
        CULog("Player is null.");
        return result;
    }

    auto player = playerData->player;

    // number of batteries/traps
    if (player->getType() == constants::PlayerType::Pal) {
        auto pal = dynamic_pointer_cast<Pal>(player);
        encodeInt(pal->getBatteries(), result);
    }
    else {
        auto ghost = dynamic_pointer_cast<Ghost>(player);
        encodeInt(ghost->getTraps(), result);
    }

    // player location
    encodeVector(player->getLoc(), result);

    // player direction
    encodeVector(player->getDir(), result);

    for (auto& p : _players) {
        auto targetPlayer = p->player;

        // player spooked / tagged
        if (_id == _hostID) {
            // is host (broadcast master data)
            if (targetPlayer->getType() == constants::PlayerType::Pal) {
                auto pal = dynamic_pointer_cast<Pal>(targetPlayer);
                pal->updateSpooked();
                encodeBool(pal->getSpooked(), result);
            }
            else {
                encodeBool(dynamic_pointer_cast<Ghost>(targetPlayer)->getTagged(), result);
            }
        }
        else {
            // is not host (broadcast data for host to update player state)
            if (player->getType() == constants::PlayerType::Pal) {
                if (targetPlayer->getType() == constants::PlayerType::Pal) {
                    // pal unspook pal
                    encodeBool(dynamic_pointer_cast<Pal>(targetPlayer)->getUnspookFlag(), result);
                }
                else {
                    // pal tag ghost
                    encodeBool(dynamic_pointer_cast<Ghost>(targetPlayer)->getTagged(), result);
                }
            }
            else {
                if (targetPlayer->getType() == constants::PlayerType::Pal) {
                    // ghost spook pal
                    encodeBool(dynamic_pointer_cast<Pal>(targetPlayer)->getSpookFlag(), result);
                }
                else {
                    // ghost kiss ghost ???
                    encodeBool(false, result);
                }
            }
        }
    }

    return result;
}

void NetworkData::interpretPlayerData(const int id, const vector<uint8_t>& playerData) {
    shared_ptr<PlayerData> otherPlayerData = getPlayer(id);

    if (otherPlayerData == nullptr || otherPlayerData->player == nullptr) {
        CULog("Other player is null.");
        return;
    }

    vector<vector<uint8_t>> splitPlayerData = split(playerData, { 4, 8, 8, 4 });
    if (splitPlayerData.empty()) return;

    auto otherPlayer = otherPlayerData->player;

    // number of batteries/traps
    if (otherPlayer->getType() == constants::PlayerType::Pal) {
        auto pal = dynamic_pointer_cast<Pal>(otherPlayer);
        pal->setBatteries(decodeInt(splitPlayerData[0]));
    }
    else {
        auto ghost = dynamic_pointer_cast<Ghost>(otherPlayer);
        ghost->setTraps(decodeInt(splitPlayerData[0]));
    }

    // location and direction
    Vec2 location = decodeVector(splitPlayerData[1]);
    Vec2 direction = decodeVector(splitPlayerData[2]);

    otherPlayer->setDir(direction);
    otherPlayerData->interpolationData->ticksSinceReceived = 0;
    otherPlayerData->interpolationData->oldPosition = otherPlayer->getLoc();
    otherPlayerData->interpolationData->newPosition = location;
    otherPlayer->setIdle((location - otherPlayer->getLoc()).length() < 1.f);

    // more complicated player data
    splitPlayerData = split(splitPlayerData[3], { 1, 1, 1, 1 });
    for (unsigned i = 0; i < _players.size(); i++) {
        auto p = _players[i];
        auto targetPlayer = p->player;

        bool data = decodeBool(splitPlayerData[i]);
        // this piece of code only works if ghost is host
        if (id == _hostID) {
            // receive master data from host
            if (targetPlayer->getType() == constants::PlayerType::Pal) {
                auto pal = dynamic_pointer_cast<Pal>(targetPlayer);
                pal->setSpooked(data);
            }
            else {
                auto ghost = dynamic_pointer_cast<Ghost>(targetPlayer);
                ghost->setTagged(data);
            }
        }
        else if (p != getPlayer()) {
            // receive data and update player state
            if (data) {
                if (otherPlayer->getType() == constants::PlayerType::Pal) {
                    if (targetPlayer->getType() == constants::PlayerType::Pal) {
                        auto pal = dynamic_pointer_cast<Pal>(targetPlayer);
                        // pal unspook pal
                        dynamic_pointer_cast<Pal>(otherPlayer)->setHelping();
                        pal->setUnspookFlag();
                    }
                }
            }
        }
    }
}

vector<uint8_t> NetworkData::convertMapData() {
    vector<uint8_t> result;
    if (_id < 0) {
        CULog("Player id is not defined.");
        return result;
    }

    if (_mapData == nullptr || _mapData->map == nullptr) {
        CULog("Game map is not defined.");
        return result;
    };

    if (_id == _hostID) {
        // traps
        vector<Vec2> trapPositions;
        vector<bool> trapTriggered;
        for (auto& trap : _mapData->map->getTraps()) {
            trapPositions.push_back(trap->getLoc());
            trapTriggered.push_back(trap->getTriggered());
        };
        encodeVec2List(trapPositions, result);
        encodeBoolList(trapTriggered, result);

        // room lights status
        vector<bool> roomsLit;
        for (auto& room : _mapData->map->getRooms()) {
            roomsLit.push_back(room->getLight());
        }
        encodeBoolList(roomsLit, result);
    }
    else {
        // room lights activated
        vector<bool> roomsLit;
        for (auto& room : _mapData->map->getRooms()) {
            if (room->getSlot() == nullptr) roomsLit.push_back(false);
            else roomsLit.push_back(room->getSlot()->activated());
        }
        encodeBoolList(roomsLit, result);

        // teleporter battery count
        encodeInt(_mapData->map->getTeleCount(), result);
    }

    return result;
}

void NetworkData::interpretMapData(const int id, const vector<uint8_t>& msg) {
    if (msg.empty()) return;

    if (_mapData == nullptr || _mapData->map == nullptr) return;

    // this only works if ghost is host
    if (id == _hostID) {
        // host data
        vector<vector<uint8_t>> splitMsg = split(msg, { 4 });
        if (splitMsg.empty()) return;

        unsigned numTraps = decodeInt(splitMsg[0]);
        vector<Vec2> trapPositions;
        if (numTraps > 0) {
            splitMsg = split(splitMsg[1], { numTraps * 8 });
            trapPositions = decodeVec2List(numTraps, splitMsg[0]);
        }
        _mapData->map->setTraps(trapPositions);

        splitMsg = split(splitMsg[1], { 4 });
        if (splitMsg.empty()) return;

        CUAssertLog(numTraps == decodeInt(splitMsg[0]), "Expected different number of traps");
        numTraps = decodeInt(splitMsg[0]);
        vector<bool> trapTriggered;
        if (numTraps > 0) {
            splitMsg = split(splitMsg[1], { numTraps * 1 });
            trapTriggered = decodeBoolList(numTraps, splitMsg[0]);
        }
        for (unsigned i = 0; i < numTraps; i++) {
            auto trap = _mapData->map->getTraps()[i];
            if (trapTriggered[i] && !trap->getTriggered()) {
                trap->setTriggered();
            }
        }

        splitMsg = split(splitMsg[1], { 4 });
        if (splitMsg.empty()) return;

        unsigned numRooms = decodeInt(splitMsg[0]);
        vector<bool> roomsLit;
        if (numRooms > 0) {
            splitMsg = split(splitMsg[1], { numRooms * 1 });
            roomsLit = decodeBoolList(numRooms, splitMsg[0]);
        }
        for (unsigned i = 0; i < numRooms && i < _mapData->map->getRooms().size(); i++) {
            auto room = _mapData->map->getRooms()[i];
            if (roomsLit[i] && !room->getLight() && room->getSlot() != nullptr) {
                room->getSlot()->setCharge();
            };
        }
    }
    else if (_id == _hostID) {
        // host receiving data
        vector<vector<uint8_t>> splitMsg = split(msg, { 4 });
        if (splitMsg.empty()) return;

        unsigned numRooms = decodeInt(splitMsg[0]);
        vector<bool> roomsLit;
        if (numRooms > 0) {
            splitMsg = split(splitMsg[1], { numRooms * 1 });
            roomsLit = decodeBoolList(numRooms, splitMsg[0]);
        }
        for (unsigned i = 0; i < numRooms && i < _mapData->map->getRooms().size(); i++) {
            auto room = _mapData->map->getRooms()[i];
            if (roomsLit[i] && !room->getLight() && room->getSlot() != nullptr) {
                room->getSlot()->setCharge();
            };
        }

        splitMsg = split(splitMsg[1], { 4 });
        if (splitMsg.empty()) return;

        int receivedTeleCount = decodeInt(splitMsg[0]);
        int currentTeleCount = _mapData->map->getTeleCount();
        if (currentTeleCount > receivedTeleCount) {
            _mapData->map->setTeleCount(receivedTeleCount);
        }
    }
}

vector<uint8_t> NetworkData::convertWinData() {
    vector<uint8_t> result;
    if (_id < 0) {
        CULog("ID is not defined.");
        return result;
    }

    uint8_t winner = 0;

    if (_id == _hostID) {
        if (_winData != nullptr) {
            switch (_winData->winner) {
            case constants::PlayerType::Undefined:
                break;
            case constants::PlayerType::Pal:
                winner = 1;
                break;
            case constants::PlayerType::Ghost:
                winner = 2;
                break;
            }
        }
    }
    encodeByte(winner, result);

    return result;
}

void NetworkData::interpretWinData(const int id, const vector<uint8_t>& msg) {
    vector<vector<uint8_t>> splitMsg = split(msg, { 1 });
    if (splitMsg.empty()) return;
    if (id == _hostID) {
        switch (decodeByte(splitMsg[0])) {
        case 0:
            break;
        case 1:
            setWinner(constants::PlayerType::Pal);
            break;
        case 2:
            setWinner(constants::PlayerType::Ghost);
            break;
        }
    }
}

vector<uint8_t> NetworkData::serializeData() {
    vector<uint8_t> result;

    vector<uint8_t> metadata = convertMetadata();
    result.insert(result.end(), metadata.begin(), metadata.end()); // 5

    switch (_status) {
    case constants::MatchStatus::Waiting: {
        vector<uint8_t> lobbyData = convertLobbyData();
        result.insert(result.end(), lobbyData.begin(), lobbyData.end()); // 16

        vector<uint8_t> mapLayoutData = convertMapLayoutData();
        result.insert(result.end(), mapLayoutData.begin(), mapLayoutData.end()); // struct
        break;
    }
    case constants::MatchStatus::InProgress: {
        vector<uint8_t> playerData = convertPlayerData(); // 24
        result.insert(result.end(), playerData.begin(), playerData.end());

        vector<uint8_t> mapData = convertMapData(); // variable
        result.insert(result.end(), mapData.begin(), mapData.end());
        break;
    }
    case constants::MatchStatus::Paused:
        break;
    case constants::MatchStatus::Ended:
        vector<uint8_t> winData = convertWinData(); // 1
        result.insert(result.end(), winData.begin(), winData.end());
        break;
    }

    return result;
}

void NetworkData::unserializeData(const vector<uint8_t>& msg) {
    vector<vector<uint8_t>> splitMsg = split(msg, { 5 });
    if (splitMsg.empty()) return;

    shared_ptr<NetworkMetadata> metadata = interpretMetadata(splitMsg[0]);
    auto status = metadata->status;
    if (status != _status) {
        if (metadata->id == _hostID) _status = metadata->status; // sync with host status
        else return;
    }

    switch (_status) {
    case constants::MatchStatus::None:
        break;
    case constants::MatchStatus::Waiting: {
        if (splitMsg.size() <= 1) break;
        splitMsg = split(splitMsg[1], { 16 });
        if (splitMsg.empty()) break;
        interpretLobbyData(metadata->id, splitMsg[0]);
        if (splitMsg.size() < 2) break;
        interpretMapLayoutData(metadata->id, splitMsg[1]);
        break;
    }
    case constants::MatchStatus::InProgress: {
        if (splitMsg.size() <= 1) break;
        splitMsg = split(splitMsg[1], { 24 });
        if (splitMsg.empty()) break;
        interpretPlayerData(metadata->id, splitMsg[0]);
        if (splitMsg.size() < 2) break;
        interpretMapData(metadata->id, splitMsg[1]);
        break;
    }
    case constants::MatchStatus::Paused:
        break;
    case constants::MatchStatus::Ended:
        if (splitMsg.size() <= 1) break;
        interpretWinData(metadata->id, splitMsg[1]);
        break;
    }
}

void NetworkData::interpolatePlayerData() {
    vector<shared_ptr<PlayerData>> otherPlayers;
    for (auto& p : _players) {
        if (p != nullptr && p->id >= 0 && p->id != _id) {
            otherPlayers.push_back(p);
        }
    }

    for (auto& p : otherPlayers) {
        float progress = min((float)p->interpolationData->ticksSinceReceived / constants::NETWORK_TICKS, 1.f);
        Vec2 interpolatedPosition = p->interpolationData->oldPosition * (1 - progress) + p->interpolationData->newPosition * progress;

        p->player->setLoc(interpolatedPosition);
        p->player->update();
        if (progress >= 1.f) {
            p->player->setIdle(true);
        }

        ++p->interpolationData->ticksSinceReceived;
    }
}