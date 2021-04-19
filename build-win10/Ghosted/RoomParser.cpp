#include "RoomParser.h"
#include "GameRoom.h"

using namespace cugl;


/** Takes in a json file and creates a room object */
shared_ptr<GameRoom> RoomParser::parse(string file, const Vec2& origin) {
	shared_ptr<JsonReader> reader = JsonReader::alloc(file);
	shared_ptr<JsonValue> value = reader->readJson();
	shared_ptr<GameRoom> room = GameRoom::alloc(origin);
	
	shared_ptr<JsonValue> batteries = value->get("batteries");
	vector<vector<int>> batteryList = batteries->asVecArray(*batteries);

	//room->setBatterySpawns(batteryList);
	return room;
}

