#pragma once

#ifndef __GAME_ROOM_H__
#define __GAME_ROOM_H__
#include <cugl/cugl.h>

using namespace cugl;

constexpr int ROOM_DIMENSION = 600;

using namespace std;
using namespace cugl;

/** Class modeling a game room. */
class GameRoom {
private:
	shared_ptr<Scene2> _roomScene;

	shared_ptr<Texture> _tileTexture;
	shared_ptr<Texture> _wallTexture;

	shared_ptr<scene2::PolygonNode> _node;

	// (0,0) at bottom left of map
	Vec2 _origin;

	// List of possible spawn rooms
	vector<vector<int>> _batterySpawns;

	// doors lead to connected rooms
	set<shared_ptr<GameRoom>> _connectedRooms;

	// List of bools that represent which side has a door and which doesn't. In order of North, South, East, and West
	vector<bool> _doors;

	bool assertValidRoom();

	bool assertRoomIsAdjacent(const shared_ptr<GameRoom>& room);

public:
	GameRoom() {}

	~GameRoom() { }

	bool init(const Vec2& origin);

	static shared_ptr<GameRoom> alloc() {
		shared_ptr<GameRoom> result = make_shared<GameRoom>();
		return (result->init(Vec2(0,0)) ? result : nullptr);
	}

	static shared_ptr<GameRoom> alloc(const Vec2& origin) {
		shared_ptr<GameRoom> result = make_shared<GameRoom>();
		return (result->init(origin) ? result : nullptr);
	}

	// Gets the coordinates of the origin of this room
	Vec2 getOrigin() { return _origin; }
	
	// Gets the set of rooms connected to the current room
	set<shared_ptr<GameRoom>> getConnectedRooms() { return _connectedRooms; };

	// Gets the animation node
	shared_ptr<scene2::PolygonNode> getNode() { return _node; };

	// Gets the list of battery spawns
	vector<vector<int>> getBatterySpawns() { return _batterySpawns; };

	// Gets the list of doors
	vector<bool> getDoors() { return _doors; };

	// Sets the list of doors
	void setDoors(vector<bool> doors) { _doors = doors; };

	// Sets the animation node
	void setNode(const shared_ptr<scene2::PolygonNode>& value) { _node = value; };

	// Sets tile and wall textures of the room
	bool setTextures();

	// Sets the list of battery spawns
	void setBatterySpawns(vector<vector<int>> spawns) { _batterySpawns = spawns; };

	// Sets the inputted room as a room connected to the current one
	bool setConnectedRoom(const shared_ptr<GameRoom>& room);

	friend bool operator<(const shared_ptr<GameRoom>& l, const shared_ptr<GameRoom>& r) {
		if (l->_origin.x != r->_origin.x) {
			return l->_origin.x < r->_origin.x;
		}
		else if (l->_origin.y != r->_origin.y) {
			return l->_origin.y < r->_origin.y;
		}
		return false;
	}
};

#endif /** __GAME_ROOM_H__ */