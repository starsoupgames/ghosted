#include "GameMap.h"

bool GameMap::assertValidMap() {
	// make sure there are no overlapping rooms
	set<Vec2> origins;
	for (vector<GameRoom>::iterator room = _rooms.begin(); room != _rooms.end(); ++room) {
		set<Vec2>::iterator it = origins.find(room->getOrigin());
		if (it != origins.end()) return false;
		origins.insert(room->getOrigin());
	}
	// make sure connections are two-way
	for (vector<GameRoom>::iterator room1 = _rooms.begin(); room1 != _rooms.end(); ++room1) {
		set<GameRoom> connectedRooms1 = room1->getConnectedRooms();
		for (vector<GameRoom>::iterator room2 = room1; room2 != _rooms.end(); ++room2) {
			if (connectedRooms1.find(*room2) != connectedRooms1.end()) {
				set<GameRoom> connectedRooms2 = room2->getConnectedRooms();
				if (connectedRooms2.find(*room1) == connectedRooms2.end()) return false;
			};
		}
	}
	return true;
};

bool GameMap::generateBasicMap() {
	GameMap::reset();
	GameRoom room1(Vec2(0, 0));
	GameRoom room2(Vec2(600, 0));
	GameRoom room3(Vec2(1200, 0));
	_rooms.push_back(room1);
	_rooms.push_back(room2);
	_rooms.push_back(room3);
	room1.setConnectedRoom(room2);
	room2.setConnectedRoom(room1);
	room2.setConnectedRoom(room3);
	room3.setConnectedRoom(room2);
	return GameMap::assertValidMap();
};