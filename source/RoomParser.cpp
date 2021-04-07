#include "RoomParser.h"

using namespace cugl;

/** Takes in a json file and creates a room object */
GameRoom parse(string file) {
	shared_ptr<JsonReader> reader = JsonReader::alloc(file);
	shared_ptr<JsonValue> value = reader->readJson();
	shared_ptr<JsonValue> test = value->get("obstacles");
	CULog(test->toString().c_str());
}