#pragma once

#include <cugl/cugl.h>
#include "GameRoom.h"
using namespace std;
using namespace cugl;

/**
* This class represents the room parser. It's responsible for
* takin in json representations of rooms and creating room objects
* as per the json's specifications
*/
class RoomParser {

public:
	/** Creates a new Parser with the default values */
	RoomParser() {};

	/** Releases all resources allocated with this Parser */
	~RoomParser() {};

	/** Initializes a new Parser */
	bool init() {};

	/** Takes in a json file and creates a room object */
	shared_ptr<GameRoom> parse(string file, const Vec2& origin);

};