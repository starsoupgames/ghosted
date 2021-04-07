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
	vector<shared_ptr<GameRoom>> _rooms;

	bool assertValidMap();

	bool generateBasicMap();

	// all the nodes for the rooms
	vector<shared_ptr<scene2::PolygonNode>> _nodes;

public:
	GameMap() { }

	~GameMap() { }

	bool init();

	void reset() { _rooms.clear(); }

	bool generateRandomMap();

	vector<shared_ptr<scene2::PolygonNode>> getNodes() { return _nodes; }

	bool setTextures(vector<shared_ptr<Texture>> textures);
};

#endif /** __GAME_MAP_H__ */