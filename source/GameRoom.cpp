#include "GameRoom.h"

using namespace std;
using namespace cugl;

bool GameRoom::init(const shared_ptr<AssetManager>& assets, const shared_ptr<scene2::OrderedNode>& node, const Vec2& origin, vector<bool> doors) {
    _assets = assets;
    _node = node;
    _ranking = Vec2(origin.x / 1120, origin.y / 1120);
    _origin = origin;
    _batterySpawns = vector<vector<int>>();
    _doors = doors;

    auto slotNode = scene2::PolygonNode::allocWithTexture(_assets->get<Texture>("slot_empty"));
    slotNode->setScale(0.05f);
    slotNode->setPosition(Vec2(ROOM_DIMENSION / 2, ROOM_DIMENSION / 2));
    slotNode->setPriority(constants::Priority::RoomEntity);
    _node->addChild(slotNode);

    _slotModel = BatterySlot::alloc(Vec2(ROOM_DIMENSION / 2, ROOM_DIMENSION / 2));
    _slotModel->setNode(slotNode);

    // set obstacle nodes based on the passed in json

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
