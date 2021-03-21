#pragma once

#ifndef __GAME_MAP_H__
#define __GAME_MAP_H__
#include <cugl/cugl.h>
#include "GameRoom.h"

using namespace std;
using namespace cugl;

/** Class modeling a game map. */
class GameMap {
private:
	shared_ptr<Scene2> _mapScene;

	// default textures if specific room textures are not specified
	shared_ptr<Texture> _tileTexture;
	shared_ptr<Texture> _wallTexture;

	// all the rooms in the map
	vector<GameRoom> _rooms;

	bool assertValidMap();

	bool generateBasicMap();

	bool generateRandomMap();

public:
	GameMap() { }

	~GameMap() { }

	bool init();

	void reset() { _rooms.clear(); }

};

#endif /** __GAME_MAP_H__ */