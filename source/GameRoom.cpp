#include "GameRoom.h"

using namespace std;
using namespace cugl;

bool GameRoom::init(const shared_ptr<AssetManager>& assets, const shared_ptr<scene2::OrderedNode>& node, const Vec2& origin, vector<bool> doors, Vec2 ranking) {
    _assets = assets;
    _node = node;
    _ranking = ranking;
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
};

bool GameRoom::init(const shared_ptr<AssetManager>& assets, const Vec2& origin, vector<bool> doors, Vec2 ranking) {
    _assets = assets;
    _doors = doors;
    _origin = origin;
    _ranking = ranking;

    // set obstacle nodes based on the passed in json

    return true;
}

void GameRoom::setNode(const shared_ptr<scene2::OrderedNode>& value) {
    _node = value;
    auto slotNode = scene2::PolygonNode::allocWithTexture(_assets->get<Texture>("slot_empty"));
    slotNode->setScale(0.05f);
    slotNode->setPosition(Vec2(ROOM_DIMENSION / 2, ROOM_DIMENSION / 2));
    slotNode->setPriority(constants::Priority::RoomEntity);
    _node->addChild(slotNode);

    _slotModel = BatterySlot::alloc(Vec2(ROOM_DIMENSION / 2, ROOM_DIMENSION / 2));
    _slotModel->setNode(slotNode);
};

void GameRoom::setRoot(const shared_ptr<scene2::OrderedNode>& value) {
    _root = value;
};

void GameRoom::addObstacles(const shared_ptr<RoomParser>& parser, bool end) {
    auto litRoot = _root->getChildByName("lit_root");
    auto dimRoot = _root->getChildByName("dim_root");
    auto topRoot = _root->getChildByName("top_root");

    shared_ptr<Texture> floorTexture = _assets->get<Texture>("dim_floor1_texture");
    shared_ptr<Texture> litFloorTexture = _assets->get<Texture>("lit_floor1_texture");
    string roomCode = getDoorsStr();
    shared_ptr<Texture> doorTexture = _assets->get<Texture>(roomCode);
    shared_ptr<scene2::PolygonNode> node = scene2::PolygonNode::allocWithTexture(doorTexture);

    shared_ptr<scene2::PolygonNode> floorNode = scene2::PolygonNode::allocWithTexture(floorTexture);
    //offset for transformation matrix
    Mat4 floorOffset = Mat4::createTranslation(Vec3(80, 0, 0));
    floorNode->setAnchor(Vec2::ANCHOR_BOTTOM_LEFT);
    floorNode->setAlternateTransform(floorOffset);
    floorNode->chooseAlternateTransform(true);
    node->addChild(floorNode);
    node->setAnchor(Vec2::ANCHOR_BOTTOM_LEFT);
    dimRoot->addChild(node);
    node->setPosition(getOrigin());

    // Ranking used to offset positions
    //int xOffset = (ranking.x * 1120) + 80;
    //int yOffset = ranking.y * 1120;

    // Quick fix to offset bug using magic numbers, change after Golden Master
    Vec2 offset = Vec2(100, 80);

    // Draw furniture
    string path;
    if (end) {
        CULog("building end room");
        path = "json/layouts/end.json";
    }
    else {
        path = parser->pickLayout(roomCode);
    }
    shared_ptr<LayoutMetadata> roomData = parser->getLayoutData(path);
    for (auto& obs : roomData->obstacles) {
        // Get texture with name
        shared_ptr<Texture> obsTexture = _assets->get<Texture>(obs.name);
        shared_ptr<scene2::PolygonNode> obsNode = scene2::PolygonNode::allocWithTexture(obsTexture);
        // Flip if required
        obsNode->flipHorizontal(obs.flip);
        // Get position, multiply by pixel size of each tile, and then add the offsets
        Vec2 position = Vec2((obs.position.x * constants::TILE_SIZE), (obs.position.y * constants::TILE_SIZE)).add(_origin) + offset;
        obsNode->setPosition(position);
        // Do something with hitboxes
        Vec2 hitbox = obs.hitbox;
        // Add to scene graph
        topRoot->addChild(obsNode);
    }

    //scene graph for lit versions of entities, or entities only visible under light
    shared_ptr<scene2::PolygonNode> litFloorNode = scene2::PolygonNode::allocWithTexture(litFloorTexture);
    litFloorNode->setAlternateTransform(floorOffset);
    litFloorNode->chooseAlternateTransform(true);
    litFloorNode->setAnchor(Vec2::ANCHOR_BOTTOM_LEFT);

    shared_ptr<scene2::PolygonNode> litDoorNode = scene2::PolygonNode::allocWithTexture(doorTexture);
    litDoorNode->addChild(litFloorNode);
    litDoorNode->setAnchor(Vec2::ANCHOR_BOTTOM_LEFT);
    _node->addChild(litDoorNode);
    litDoorNode->setPosition(getOrigin());

};

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
