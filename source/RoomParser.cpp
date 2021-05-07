#include "RoomParser.h"
#include "GameRoom.h"

using namespace cugl;

/** Takes in a json file and creates a room object */
shared_ptr<GameRoom> RoomParser::parse(string file, const Vec2& origin) {
    shared_ptr<JsonReader> reader = JsonReader::alloc(file);
    shared_ptr<JsonValue> value = reader->readJson();
    // shared_ptr<JsonValue> obsctacles = value->get("obstacles");
    shared_ptr<GameRoom> room = GameRoom::alloc(origin, { true, true, true, true });
    return room;
}
