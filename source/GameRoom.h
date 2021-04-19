#pragma once

#ifndef __GAME_ROOM_H__
#define __GAME_ROOM_H__
#include <cugl/cugl.h>
#include "RoomEntities/Trap.h"
#include "RoomEntities/BatterySlot.h"

using namespace cugl;

// prev value: 600 (outdated?)
constexpr int ROOM_DIMENSION = 375;

using namespace std;
using namespace cugl;

/** Class modeling a game room. 
 * GameScene needs to be able to get information from GameRoom in order
 * to create the Box2d objects.
 * 
 * GameMap needs to be able to change the trap node's visibility, whether to
 * draw a filled in battery slot or empty, 
 */
class GameRoom {
private:
	/** The node containing the nodes for the floor + wall, slot, trap, cable,
	 * and obstacles.
	 */
	shared_ptr<scene2::PolygonNode> _node;

	/** The nodes for the floor + wall, slot, trap, and cable */
	// These should only be alloc'd with position, as textures will be set in GameScene
	shared_ptr<scene2::PolygonNode> _roomNode;
	shared_ptr<BatterySlot> _slotModel;
	shared_ptr<Trap> _trapModel;
	shared_ptr<scene2::PolygonNode> _cableNode;

	/** The origin of the room. Distance from (0,0) of the map to room's bottom left corner */
	Vec2 _origin;

	/** Possible locations within the room where batteries can spawn */
	vector<vector<int>> _batterySpawns;

	// List of bools that represent which side has a door and which doesn't. In order of North, East, South, and West
	vector<bool> _doors;

	bool assertValidRoom();

	bool assertRoomIsAdjacent(const shared_ptr<GameRoom>& room);

public:
	GameRoom() {}

	~GameRoom() { }

	bool init(const Vec2& origin);
    
    void initContents(shared_ptr<Texture> trap, shared_ptr<Texture> slot, Size size);

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

	// Gets the scene node
	shared_ptr<scene2::PolygonNode> getNode() { return _node; };
    
    void setNode(shared_ptr<scene2::PolygonNode> node) {
        _node = node;
    }

	// Gets the list of battery spawns
	vector<vector<int>> getBatterySpawns() { return _batterySpawns; };

	// Gets the list of doors
	vector<bool> getDoors() { return _doors; };
    
    shared_ptr<Trap> getTrap() { return _trapModel; };
    
    shared_ptr<BatterySlot> getSlot() { return _slotModel; };
    
    bool getLight() { return _slotModel->getCharge() > 0;} 

	// Comparison operator
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
