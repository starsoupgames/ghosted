#include "NetworkData.h"
#include "Utils.h"

using namespace std;
using namespace cugl;

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

/** Split byte vector at certain points */
vector<vector<uint8_t>> split(const vector<uint8_t>& bytes, const vector<uint8_t>& sizes) {
    vector<vector<uint8_t>> result;
    uint8_t index = 0;
    for (auto& size : sizes) {
        if (bytes.size() < index + size) {
            CULogError("Byte vector is too small.");
            return result;
        }
    }
    for (auto& size : sizes) {
        result.push_back(vector(bytes.begin() + index, bytes.begin() + index + size));
        index += size;
    }
    if (index != bytes.size()) {
        result.push_back(vector(bytes.begin() + index, bytes.begin() + bytes.size()));
    }
    return result;
}

void NetworkData::encodeByte(uint8_t b, vector<uint8_t>& out) {
    out.push_back(b);
}

uint8_t NetworkData::decodeByte(const vector<uint8_t>& bytes) {
    CUAssertLog(bytes.size() == 1, "decodeByte: Byte vector size is not equal to data type size.");
    return bytes[0];
}

void NetworkData::encodeInt(int i, vector<uint8_t>& out) {
    int temp = marshall(i);
    unsigned char bytes[4];
    bytes[0] = (temp >> 24) & 0xFF;
    bytes[1] = (temp >> 16) & 0xFF;
    bytes[2] = (temp >> 8) & 0xFF;
    bytes[3] = temp & 0xFF;
    out.insert(out.end(), begin(bytes), end(bytes));
}

int NetworkData::decodeInt(const vector<uint8_t>& bytes) {
    CUAssertLog(bytes.size() == sizeof(int), "decodeInt: Byte vector size is not equal to data type size.");
    int32_t i = (bytes[0] << 24) + (bytes[1] << 16) + (bytes[2] << 8) + bytes[3];
    return marshall(i);
}

void NetworkData::encodeFloat(float f, vector<uint8_t>& out) {
    encodeInt(static_cast<int>(f * FLOAT_PRECISION), out);
}

float NetworkData::decodeFloat(const vector<uint8_t>& bytes) {
    return static_cast<float>(decodeInt(bytes)) / FLOAT_PRECISION;
}

void NetworkData::encodeVector(const Vec2& v, vector<uint8_t>& out) {
    encodeFloat(v.x, out);
    encodeFloat(v.y, out);
}

Vec2 NetworkData::decodeVector(const vector<uint8_t>& bytes) {
    CUAssertLog(bytes.size() == 2 * sizeof(float), "decodeVector: Byte vector size is not equal to data type size.");
    vector<vector<uint8_t>> splitBytes = split(bytes, { 4, 4 });
    return Vec2(decodeFloat(splitBytes[0]), decodeFloat(splitBytes[1]));
}

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
            vector<int> playerOrder({ 0, 1, 2, 3 });
            shuffle(playerOrder.begin(), playerOrder.end(), random_device());
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
            CULog("%d", playerOrder[i]);
        }
        _lobbyData = make_shared<LobbyData>(playerOrder);
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

    // player location
    encodeVector(playerData->player->getLoc(), result);

    // player direction
    encodeVector(playerData->player->getDir(), result);

    return result;
}

void NetworkData::interpretPlayerData(const int id, const vector<uint8_t>& playerData) {
    shared_ptr<PlayerData> otherPlayer = getPlayer(id);

    vector<vector<uint8_t>> splitPlayerData = split(playerData, { 8, 8 });
    if (splitPlayerData.empty()) return;
    Vec2 location = decodeVector(splitPlayerData[0]);
    Vec2 direction = decodeVector(splitPlayerData[1]);

    if (otherPlayer == nullptr || otherPlayer->player == nullptr) {
        CULog("Other player is null.");
        return;
    }

    otherPlayer->player->setDir(direction);
    otherPlayer->interpolationData->ticksSinceReceived = 0;
    otherPlayer->interpolationData->oldPosition = otherPlayer->player->getLoc();
    otherPlayer->interpolationData->newPosition = location;
    otherPlayer->player->setIdle((location - otherPlayer->player->getLoc()).length() < 1.f);
}

vector<uint8_t> NetworkData::convertMapData() {
    vector<uint8_t> result;
    return result;
}

void NetworkData::interpretMapData(const vector<uint8_t>& msg) {

}

vector<uint8_t> NetworkData::serializeData() {
    vector<uint8_t> result;

    vector<uint8_t> metadata = convertMetadata();
    result.insert(result.end(), metadata.begin(), metadata.end()); // 5

    switch (_status) {
    case constants::MatchStatus::Waiting: {
        vector<uint8_t> lobbyData = convertLobbyData();
        result.insert(result.end(), lobbyData.begin(), lobbyData.end()); // 16
        break;
    }
    case constants::MatchStatus::InProgress: {
        vector<uint8_t> playerData = convertPlayerData(); // 16
        result.insert(result.end(), playerData.begin(), playerData.end());

        vector<uint8_t> mapData = convertMapData(); // 0
        result.insert(result.end(), mapData.begin(), mapData.end());
        break;
    }
    case constants::MatchStatus::Paused:
        break;
    case constants::MatchStatus::Ended:
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
        return;
    }

    switch (_status) {
    case constants::MatchStatus::None:
        break;
    case constants::MatchStatus::Waiting: {
        if (splitMsg.size() <= 1) break;
        interpretLobbyData(metadata->id, splitMsg[1]);
        break;
    }
    case constants::MatchStatus::InProgress: {
        if (splitMsg.size() <= 1) break;
        splitMsg = split(msg, { 5, 16, 0 });
        if (splitMsg.empty()) break;
        interpretPlayerData(metadata->id, splitMsg[1]);
        interpretMapData(splitMsg[2]);
        break;
    }
    case constants::MatchStatus::Paused:
        break;
    case constants::MatchStatus::Ended:
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
        /*
        Vec2 diff = interpolatedPosition - otherPlayer->getLoc();
        utils::Log(diff.length());
        if (diff.isNearZero()) {
            otherPlayer->setLoc(interpolatedPosition);
            otherPlayer->setIdle(true);
        }
        else {
            otherPlayer->setMove(diff);
            otherPlayer->update();
        }*/

        p->player->setLoc(interpolatedPosition);
        p->player->update();
        if (progress >= 1.f) {
            p->player->setIdle(true);
        }

        ++p->interpolationData->ticksSinceReceived;
    }
}