#include "GameRoom.h"

using namespace std;

bool GameRoom::init(const Vec2& origin) {
	_origin = origin;
	_batterySpawns = vector<vector<int>>();
	_doors = vector<bool>{ false, false, false, false };
	return true;
}

bool GameRoom::assertValidRoom() {
	if ((int)_origin.x % ROOM_DIMENSION != 0) return false;
	if ((int)_origin.y % ROOM_DIMENSION != 0) return false;
	return true;
};

bool GameRoom::assertRoomIsAdjacent(const shared_ptr<GameRoom>& room) {
	Vec2 diff = _origin - room->_origin;
	// check if within square
	if (diff.x <= ROOM_DIMENSION && diff.y <= ROOM_DIMENSION) {
		// return false if horizontal diff
		return abs(diff.x) == abs(diff.y);
	}
	return false;
};

bool GameRoom::setConnectedRoom(const shared_ptr<GameRoom>& room) {
	if (!GameRoom::assertRoomIsAdjacent(room)) return false;
	if (_connectedRooms.size() >= 4) return false;
	_connectedRooms.insert(room);
	if (_origin.x < room->getOrigin().x) {
		// Door on the east
		_doors[2] = true;
		// Create complimentary result
		vector<bool> temp = room->getDoors();
		temp[3] = true;
		room->setDoors(temp);
	}
	else if (_origin.x > room->getOrigin().x) {
		// Door on the west
		_doors[3] = true;
		// Create complimentary result
		vector<bool> temp = room->getDoors();
		temp[2] = true;
		room->setDoors(temp);
	}
	else if (_origin.y < room->getOrigin().y) {
		// Door on the north
		_doors[0] = true;
		// Create complimentary result
		vector<bool> temp = room->getDoors();
		temp[1] = true;
		room->setDoors(temp);
	}
	else if (_origin.y > room->getOrigin().y) {
		// Door on the south
		_doors[1] = true;
		// Create complimentary result
		vector<bool> temp = room->getDoors();
		temp[0] = true;
		room->setDoors(temp);
	}
	return true;
}