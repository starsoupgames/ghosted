#pragma once

#ifndef __GAME_ROOM_H__
#define __GAME_ROOM_H__
#include <cugl/cugl.h>

constexpr int ROOM_DIMENSION = 600;

using namespace std;
using namespace cugl;

/** Class modeling a game room. */
class GameRoom {
private:
	shared_ptr<Scene2> _roomScene;

	shared_ptr<Texture> _tileTexture;
	shared_ptr<Texture> _wallTexture;

	// (0,0) at bottom left of map
	Vec2 _origin;

	// doors lead to connected rooms
	set<GameRoom> _connectedRooms;

	bool assertValidRoom();

	bool assertRoomIsAdjacent(GameRoom& room);

public:
	GameRoom(Vec2& origin) : _origin(origin) { }

	~GameRoom() { }

	bool init();

	// set tile and wall textures of the room
	bool setTextures();

	Vec2 getOrigin() { return _origin; }

	set<GameRoom>& getConnectedRooms() { return _connectedRooms; };

	bool setConnectedRoom(GameRoom& room);

	friend bool operator<(const GameRoom& l, const GameRoom& r) {
		if (l._origin.x != r._origin.x) {
			return l._origin.x < r._origin.x;
		}
		else if (l._origin.y != r._origin.y) {
			return l._origin.y < r._origin.y;
		}
		return false;
	}
};

#endif /** __GAME_ROOM_H__ */