#include "GameRoom.h"

using namespace std;

bool GameRoom::init(const Vec2& origin, vector<bool> doors) {
	// traps should be setVisible(false)
	// slots should be setVisible(true)
	// set obstacle nodes based on the passed in json
    _ranking = Vec2(origin.x / 1120, origin.y / 1120);
	_origin = origin;
	_batterySpawns = vector<vector<int>>();
	_doors = doors;
	return true;
}

void GameRoom::initContents(shared_ptr<Texture> slot, Size size) {
    Vec2 center = Vec2(ROOM_DIMENSION/2, ROOM_DIMENSION/2);
    _slotModel = BatterySlot::alloc(_origin);
    _slotModel->setTextures(slot, size/2);
    _slotModel->getNode()->setScale(0.05f);
    _slotModel->getNode()->setPosition(center);
    _node->addChild(_slotModel->getNode());
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
