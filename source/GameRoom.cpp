#include "GameRoom.h"

using namespace std;

bool GameRoom::init(const Vec2& origin) {
	// traps should be setVisible(false)
	// slots should be setVisible(true)
	// set obstacle nodes based on the passed in json
    
	_origin = origin;
	_batterySpawns = vector<vector<int>>();
	_doors = vector<bool>{ false, false, false, false };
	return true;
}

void GameRoom::initContents(shared_ptr<Texture> trap, shared_ptr<Texture> slot, Size size) {
    Vec2 center = Vec2(ROOM_DIMENSION/2, ROOM_DIMENSION/2);
    _slotModel = BatterySlot::alloc(_origin);
    _slotModel->setTextures(slot, size/2);
    _slotModel->getNode()->setScale(0.05f);
    _slotModel->getNode()->setPosition(center);
    _node->addChild(_slotModel->getNode());
    
    _trapModel = Trap::alloc(_origin);
    _trapModel->setTextures(trap, size);
    _trapModel->getNode()->setScale(0.25f);
    _trapModel->getNode()->setPosition(center);
    _trapModel->getNode()->setVisible(false);
    _node->addChild(_trapModel->getNode());
    _trapModel->setArmed(true);
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
