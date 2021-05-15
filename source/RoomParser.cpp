#include "RoomParser.h"
#include "GameRoom.h"
#include <random>

using namespace cugl;

/** Returns the index to a random element of a list of floats */
int RoomParser::getRandIndex(vector<float> lst) {
    // https://cpppatterns.com/patterns/choose-random-element.html
    std::random_device random_device;
    std::mt19937 engine{ random_device() };
    std::uniform_int_distribution<int> dist(0, lst.size() - 1);
    int random_element = lst[dist(engine)];
    return random_element;
}


/** Takes in a door configuration and returns the path to an obstacle layout */
string RoomParser::pickLayout(string code) {
    shared_ptr<JsonReader> reader = JsonReader::alloc("json/layouts/layouts.json");
    shared_ptr<JsonValue> value = reader->readJson();
    vector<float> layouts = value->get(code)->asFloatArray();

    return "json/layouts/" + to_string(getRandIndex(layouts)) + ".json";
}

/** Returns the path to a random map layout */
string RoomParser::pickMap() {
    shared_ptr<JsonReader> reader = JsonReader::alloc("json/maps/mapcount.json");
    shared_ptr<JsonValue> value = reader->readJson();
    vector<float> layouts = value->get("maps")->asFloatArray();

    return "json/maps/map" + to_string(getRandIndex(layouts)) + ".json";
}

/** Takes in a json file and returns the layout metadata */
shared_ptr<LayoutMetadata> RoomParser::getLayoutData(string file) {
    vector<ObsMetadata> obstaclesData = {};
    vector<Vec2> spawnData = {};

    shared_ptr<JsonReader> reader = JsonReader::alloc(file);
    shared_ptr<JsonValue> value = reader->readJson();
    shared_ptr<JsonValue> obstacles = value->get("obstacles");
    for (auto& obs : obstacles->_children) {
        string name = obs->get("name")->asString();
        bool flip = obs->get("flip")->asBool();
        vector<float> pos = obs->get("position")->asFloatArray();
        vector<float> dims = obs->get("hitbox")->asFloatArray();
        Vec2 position = Vec2(pos[0], pos[1]);
        Vec2 hitbox = Vec2(dims[0], dims[1]);
        obstaclesData.push_back(ObsMetadata(name, flip, position, hitbox));
    }
    shared_ptr<JsonValue> spawns = value->get("spawns");
    for (auto& spawn : spawns->_children) {
        vector<float> pos = spawn->get("position")->asFloatArray();
        Vec2 position = Vec2(pos[0], pos[1]);
        spawnData.push_back(position);
    }
    return make_shared<LayoutMetadata>(obstaclesData, spawnData);
}

/** Takes in a json file and returns the corresponding map */
shared_ptr<MapMetadata> RoomParser::getMapData(string file) {
    vector<RoomMetadata> rooms = {};
    vector<vector<Vec2>> spawns = {};

    shared_ptr<JsonReader> reader = JsonReader::alloc(file);
    shared_ptr<JsonValue> value = reader->readJson();

    // Start with rooms
    shared_ptr<JsonValue> roomData = value->get("rooms");
    for (auto& room : roomData->_children) {
        vector<bool> doors = room->get("doors")->asBoolArray();
        vector<float> temp = room->get("rank")->asFloatArray();
        Vec2 rank = Vec2(temp[0], temp[1]);
        rooms.push_back(RoomMetadata(doors, rank));
    }
    // Then add spawns
    shared_ptr<JsonValue> spawnData = value->get("spawns");
    for (auto& pos : spawnData->_children) {
        vector<Vec2> pair = {};
        vector<float> temp = pos->get("start")->asFloatArray();
        pair.push_back(Vec2(temp[0], temp[1]));
        temp = pos->get("end")->asFloatArray();
        pair.push_back(Vec2(temp[0], temp[1]));

        spawns.push_back(pair);
    }
    return make_shared<MapMetadata>(rooms, spawns);
}