#include "GameRoom.h"

bool GameRoom::assertValidRoom() {
	if ((int)_origin.x % ROOM_DIMENSION != 0) return false;
	if ((int)_origin.y % ROOM_DIMENSION != 0) return false;
	return true;
};

bool GameRoom::assertRoomIsAdjacent(GameRoom& room) {
	Vec2 diff = _origin - room._origin;
	// check if within square
	if (diff.x <= ROOM_DIMENSION && diff.y <= ROOM_DIMENSION) {
		// return false if horizontal diff
		return abs(diff.x) == abs(diff.y);
	}
	return false;
};

bool GameRoom::setConnectedRoom(GameRoom& room) {
	if (!GameRoom::assertRoomIsAdjacent(room)) return false;
	if (_connectedRooms.size() >= 4) return false;
	_connectedRooms.insert(room);
	return true;
}