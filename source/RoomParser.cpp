#include "RoomParser.h"
#include "GameRoom.h"
#include <random>

using namespace cugl;

template <typename T>

/** Returns the index to a random element of a list of floats */
int RoomParser::getRandIndex(vector<T> lst) {
    // https://cpppatterns.com/patterns/choose-random-element.html
    std::random_device random_device;
    std::mt19937 engine{ random_device() };
    std::uniform_int_distribution<int> dist(0, lst.size() - 1);
    //int random_element = lst[dist(engine)];
    return dist(engine);
}


/** Takes in a door configuration and returns the path to an obstacle layout */
string RoomParser::pickLayout(string code) {
    shared_ptr<JsonReader> reader = JsonReader::alloc("json/layouts/layouts.json");
    shared_ptr<JsonValue> value = reader->readJson();
    vector<int> layouts = value->get(code)->asIntArray();

    return "json/layouts/" + to_string(layouts[getRandIndex(layouts)]) + ".json";
}

/** Returns the path to a random map layout */
string RoomParser::pickMap() {
    shared_ptr<JsonReader> reader = JsonReader::alloc("json/maps/mapcount.json");
    shared_ptr<JsonValue> value = reader->readJson();
    vector<int> layouts = value->get("maps")->asIntArray();

    return "json/maps/map" + to_string(layouts[getRandIndex(layouts)]) + ".json";
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

/** Takes in a json file and returns the corresponding map.
 *  Also picks a start room and end room combination.
 */
shared_ptr<MapMetadata> RoomParser::getMapData(string file) {
    vector<RoomMetadata> rooms = {};

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
    // Then pick a spawn

    vector<shared_ptr<JsonValue>> spawnData = value->get("spawns")->_children;
    shared_ptr<JsonValue> randSpawn = spawnData[getRandIndex(spawnData)];
    Vec2 start = Vec2(randSpawn->get("start")->asIntArray()[0], randSpawn->get("start")->asIntArray()[1]);
    Vec2 end = Vec2(randSpawn->get("end")->asIntArray()[0], randSpawn->get("end")->asIntArray()[1]);
    return make_shared<MapMetadata>(rooms, start, end);

}