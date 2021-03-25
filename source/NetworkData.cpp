#include "NetworkData.h"

using namespace std;
using namespace cugl;

bool NetworkData::init() {
    _id = 0;
    return true;
}

vector<vector<uint8_t>> NetworkData::split(const vector<uint8_t>& bytes, const vector<uint8_t>& sizes) {
    vector<vector<uint8_t>> result;
    uint8_t index = 0;
    for (auto& size : sizes) {
        CUAssertLog(bytes.size() >= index + size, "Byte vector is too small.");
        result.push_back(vector(bytes.begin() + index, bytes.begin() + index + size));
        index += size;
    }
    return result;
}

void NetworkData::encodeByte(uint8_t b, vector<uint8_t>& out) {
    out.push_back(b);
}

uint8_t NetworkData::decodeByte(const vector<uint8_t>& bytes) {
    CUAssertLog(bytes.size() == 1, "Byte vector size is not equal to data type size.");
    return bytes[0];
}

void NetworkData::encodeFloat(float f, vector<uint8_t>& out) {
    int i = marshall(static_cast<int>(f * FLOAT_PRECISION));
    unsigned char bytes[4];
    bytes[0] = (i >> 24) & 0xFF;
    bytes[1] = (i >> 16) & 0xFF;
    bytes[2] = (i >> 8) & 0xFF;
    bytes[3] = i & 0xFF;
    out.insert(out.end(), begin(bytes), end(bytes));
}

float NetworkData::decodeFloat(const vector<uint8_t>& bytes) {
    CUAssertLog(bytes.size() == sizeof(float), "Byte vector size is not equal to data type size.");
    int32_t i = (bytes[0] << 24) + (bytes[1] << 16) + (bytes[2] << 8) + bytes[3];
    return static_cast<float>(marshall(i)) / FLOAT_PRECISION;
}

void NetworkData::encodeVector(Vec2 v, vector<uint8_t>& out) {
    encodeFloat(v.x, out);
    encodeFloat(v.y, out);
}

Vec2 NetworkData::decodeVector(const vector<uint8_t>& bytes) {
    CUAssertLog(bytes.size() == 2 * sizeof(float), "Byte vector size is not equal to data type size.");
    vector<vector<uint8_t>> splitBytes = split(bytes, { 4, 4 });
    return Vec2(decodeFloat(splitBytes[0]), decodeFloat(splitBytes[1]));
}

vector<uint8_t> NetworkData::convertMetadata() {
    vector<uint8_t> result;
    if (_id < 0) {
        CULog("ID is not defined.");
        return result;
    }

    // network data id
    encodeByte(_id, result);

    return result;
}


vector<uint8_t> NetworkData::convertPlayerData() {
    vector<uint8_t> result;
    if (_players[0] == nullptr) {
        CULog("Player is null.");
        return result;
    }

    // player location
    encodeVector(_players[0]->getLoc(), result);

    return result;
}

void NetworkData::interpretPlayerData(const vector<uint8_t>& metadata, const vector<uint8_t>& playerData) {
    if (_players[0] == nullptr) {
        CULog("Player is null.");
        return;
    }

    vector<vector<uint8_t>> splitMetadata = split(metadata, { 1 });
    uint8_t id = decodeByte(splitMetadata[0]);

    vector<vector<uint8_t>> splitPlayerData = split(playerData, { 8 });
    Vec2 location = decodeVector(splitPlayerData[0]);

    if (_players.size() >= 2 && _players[1] != nullptr) {
        _players[1]->setLoc(location);
    }
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
    result.insert(result.end(), metadata.begin(), metadata.end());

    vector<uint8_t> playerData = convertPlayerData();
    result.insert(result.end(), playerData.begin(), playerData.end());

    vector<uint8_t> mapData = convertMapData();
    result.insert(result.end(), mapData.begin(), mapData.end());

    return result;
}

void NetworkData::unserializeData(const vector<uint8_t>& msg) {
    vector<vector<uint8_t>> splitMsg = split(msg, { 1, 8, 0 });
    interpretPlayerData(splitMsg[0], splitMsg[1]);
    interpretMapData(splitMsg[2]);
}