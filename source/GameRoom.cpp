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

    _node->setAnchor(Vec2::ANCHOR_BOTTOM_CENTER);

    auto slotNode = scene2::PolygonNode::allocWithTexture(_assets->get<Texture>("slot_empty"));
    slotNode->setScale(0.05f);
    slotNode->setPosition(Vec2(ROOM_DIMENSION / 2, ROOM_DIMENSION / 2));
    slotNode->setPriority(constants::Priority::RoomEntity);
    _node->addChild(slotNode);

    _slotModel = BatterySlot::alloc(Vec2(ROOM_DIMENSION / 2, ROOM_DIMENSION / 2));
    _slotModel->setNode(slotNode);
    return true;
};

bool GameRoom::init(const shared_ptr<AssetManager>& assets, const Vec2& origin, vector<bool> doors, Vec2 ranking) {
    _assets = assets;
    _doors = doors;
    _origin = origin;
    _ranking = ranking;
    return true;
}

void GameRoom::setNode(const shared_ptr<scene2::OrderedNode>& value) {
    _node = value;
    _node->setPosition(getOrigin());
    _node->setAnchor(Vec2::ANCHOR_BOTTOM_CENTER);
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

void GameRoom::addObstacles(const shared_ptr<RoomParser>& parser, int end) {
    auto litRoot = _root->getChildByName("lit_root");
    auto dimRoot = _root->getChildByName("dim_root");
    auto topRoot = _root->getChildByName("top_root");

    shared_ptr<Texture> floorTexture = _assets->get<Texture>("dim_floor1_texture");
    shared_ptr<Texture> litFloorTexture = _assets->get<Texture>("lit_floor1_texture");
    string roomCode = getDoorsStr();
    shared_ptr<Texture> doorTexture = _assets->get<Texture>(roomCode);
    shared_ptr<scene2::PolygonNode> node = scene2::PolygonNode::allocWithTexture(doorTexture);
    node->setPriority(constants::Priority::Room);

    shared_ptr<scene2::PolygonNode> floorNode = scene2::PolygonNode::allocWithTexture(floorTexture);
    floorNode->setAnchor(Vec2::ANCHOR_BOTTOM_LEFT);
    floorNode->setPosition(floorNode->getPosition() + Vec2(80, 0));
    floorNode->setPriority(constants::Priority::Room);
    node->addChild(floorNode);
    node->setAnchor(Vec2::ANCHOR_BOTTOM_LEFT);
    node->setPosition(getOrigin());
    dimRoot->addChild(node);
    
    //scene graph for lit versions of entities, or entities only visible under light
    shared_ptr<scene2::PolygonNode> litFloorNode = scene2::PolygonNode::allocWithTexture(litFloorTexture);
    litFloorNode->setAnchor(Vec2::ANCHOR_BOTTOM_LEFT);
    litFloorNode->setPosition(litFloorNode->getPosition() + Vec2(80, 0));
    litFloorNode->setPriority(constants::Priority::Room);

    shared_ptr<scene2::PolygonNode> litDoorNode = scene2::PolygonNode::allocWithTexture(doorTexture);
    litDoorNode->addChild(litFloorNode);
    litDoorNode->setAnchor(Vec2::ANCHOR_BOTTOM_LEFT);
    _node->addChild(litDoorNode);
    litDoorNode->setPriority(constants::Priority::Room);

    // Draw furniture
    string path;
    if (end == 1) {
        path = "json/layouts/end.json";
        CULog("generating end room");
    }
    else if (end == 2) {
        path = "json/layouts/start.json";
        CULog("generating start room");
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
        if (obs.flip) {
            obsNode->setScale(Vec2(-1, 1));
        }
        // Get position, multiply by pixel size of each tile, and then add the offsets
        Vec2 position = Vec2((obs.position.x * constants::TILE_SIZE), (obs.position.y * constants::TILE_SIZE));
        obsNode->setPosition(position);
        obsNode->setPriority(constants::Priority::RoomEntity);
        // Do something with hitboxes
        Vec2 hitbox = obs.hitbox;
        // Add to scene graph
        litFloorNode->addChild(obsNode);

        // Now to the same for the dim version
        // Get texture with name
        shared_ptr<Texture> obsDimTexture = _assets->get<Texture>(obs.name +"_dim");
        shared_ptr<scene2::PolygonNode> obsDimNode = scene2::PolygonNode::allocWithTexture(obsDimTexture);
        // Flip if required
        if (obs.flip) {
            obsDimNode->setScale(Vec2(-1, 1));
        }
        obsDimNode->setPosition(Vec2(80, 0).add(position));
        obsDimNode->setPriority(constants::Priority::RoomEntity);
        // Add to scene graph
        node->addChild(obsDimNode);
    }

    addWalls();
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

void GameRoom::addWalls() {
    //doors in order of north east south and west
    auto topRoot = _root->getChildByName("top_root");

    int dir = 0;
    for (auto door : getDoors()) {
        switch (dir) {
        case constants::DoorDirection::North:
            {
                if (door) {
                    auto wallDimensions = Size(480, 80);
                    Vec2 wallPos = Vec2(0, constants::ROOM_DIMENSIONS.height).add(_origin);
                    auto wallRect = Rect(wallPos, wallDimensions);
                    auto wallNode = scene2::PolygonNode::alloc(wallRect);
                    wallNode->setAnchor(Vec2::ANCHOR_BOTTOM_LEFT);
                    wallNode->setColor(Color4f::BLUE);

                    wallNode->setPosition(wallPos);
                    wallNode->setPriority(constants::Priority::Room);
                    wallNode->setVisible(false);
                    topRoot->addChild(wallNode);
                    _wallNodes.push_back(wallRect);

                    Vec2 wallPos2 = Vec2((constants::ROOM_DIMENSIONS.width / 2 + 160), constants::ROOM_DIMENSIONS.height).add(_origin);
                    auto wallRect2 = Rect(wallPos2, wallDimensions);
                    auto wallNode2 = scene2::PolygonNode::alloc(wallRect2);
                    wallNode2->setAnchor(Vec2::ANCHOR_BOTTOM_LEFT);
                    wallNode2->setColor(Color4f::BLUE);

                    wallNode2->setPosition(wallPos2);
                    wallNode2->setPriority(constants::Priority::Room);
                    wallNode2->setVisible(false);
                    topRoot->addChild(wallNode2);
                    _wallNodes.push_back(wallRect2);
                    break;

                }
                else {
                    auto wallDimensions = Size(1120, 80);
                    Vec2 wallPos = Vec2(0, constants::ROOM_DIMENSIONS.height).add(_origin);
                    auto wallRect = Rect(wallPos, wallDimensions);
                    auto wallNode = scene2::PolygonNode::alloc(wallRect);
                    wallNode->setAnchor(Vec2::ANCHOR_BOTTOM_LEFT);
                    wallNode->setColor(Color4f::BLUE);

                    wallNode->setPosition(wallPos);
                    wallNode->setPriority(constants::Priority::Room);
                    wallNode->setVisible(false);
                    topRoot->addChild(wallNode);
                    _wallNodes.push_back(wallRect);
                    break;
                }
            }
        case constants::DoorDirection::East:
            {
                if (door) {
                    auto wallDimensions = Size(80, 480);
                    Vec2 wallPos = Vec2(constants::ROOM_DIMENSIONS.width + 80, -80).add(_origin);
                    auto wallRect = Rect(wallPos, wallDimensions);
                    auto wallNode = scene2::PolygonNode::alloc(wallRect);
                    wallNode->setAnchor(Vec2::ANCHOR_BOTTOM_LEFT);
                    wallNode->setColor(Color4f::BLUE);

                    wallNode->setPosition(wallPos);
                    wallNode->setPriority(constants::Priority::Room);
                    wallNode->setVisible(false);
                    topRoot->addChild(wallNode);
                    _wallNodes.push_back(wallRect);

                    Vec2 wallPos2 = Vec2(constants::ROOM_DIMENSIONS.width + 80, constants::ROOM_DIMENSIONS.height / 2 + 80).add(_origin);
                    auto wallRect2 = Rect(wallPos2, wallDimensions);
                    auto wallNode2 = scene2::PolygonNode::alloc(wallRect2);
                    wallNode2->setAnchor(Vec2::ANCHOR_BOTTOM_LEFT);
                    wallNode2->setColor(Color4f::BLUE);

                    wallNode2->setPosition(wallPos2);
                    wallNode2->setPriority(constants::Priority::Room);
                    wallNode2->setVisible(false);
                    topRoot->addChild(wallNode2);
                    _wallNodes.push_back(wallRect2);
                    break;

                }
                else {
                    //rightmost wall
                    auto wallDimensions = Size(80, 1120);
                    Vec2 wallPos = Vec2(constants::ROOM_DIMENSIONS.width, 0).add(_origin);
                    auto wallRect = Rect(wallPos, wallDimensions);
                    auto wallNode = scene2::PolygonNode::alloc(wallRect);
                    wallNode->setAnchor(Vec2::ANCHOR_BOTTOM_LEFT);
                    wallNode->setColor(Color4f::BLUE);

                    wallNode->setPosition(wallPos);
                    wallNode->setPriority(constants::Priority::Room);
                    wallNode->setVisible(false);
                    topRoot->addChild(wallNode);
                    _wallNodes.push_back(wallRect);
                    break;
                }

            }
        case constants::DoorDirection::South:
            {
                if (door) {
                    auto wallDimensions = Size(480, 80);
                    Vec2 wallPos = Vec2(0, -80).add(_origin);
                    Vec2 wallRectPos = Vec2(160, -80).add(_origin);
                    auto wallRect = Rect(wallPos, wallDimensions);
                    auto wallNode = scene2::PolygonNode::alloc(wallRect);
                    wallNode->setAnchor(Vec2::ANCHOR_BOTTOM_LEFT);
                    wallNode->setColor(Color4f::BLUE);

                    wallNode->setPosition(wallPos);
                    wallNode->setPriority(constants::Priority::Room);
                    wallNode->setVisible(false);
                    topRoot->addChild(wallNode);
                    _wallNodes.push_back(wallRect);

                    Vec2 wallPos2 = Vec2((constants::ROOM_DIMENSIONS.width / 2 + 160), -80).add(_origin);
                    Vec2 wallRectPos2 = Vec2(constants::ROOM_DIMENSIONS.width / 2 + 80, 0).add(_origin);
                    auto wallRect2 = Rect(wallPos2, wallDimensions);
                    auto wallNode2 = scene2::PolygonNode::alloc(wallRect2);
                    wallNode2->setAnchor(Vec2::ANCHOR_BOTTOM_LEFT);
                    wallNode2->setColor(Color4f::BLUE);

                    wallNode2->setPosition(wallPos2);
                    wallNode2->setPriority(constants::Priority::Room);
                    wallNode2->setVisible(false);
                    topRoot->addChild(wallNode2);
                    _wallNodes.push_back(wallRect2);
                    break;
                }
                else {
                    //bottommost wall
                    auto wallDimensions = Size(1120, 80);
                    Vec2 wallPos = Vec2(0, -80).add(_origin);
                    auto wallRect = Rect(wallPos, wallDimensions);
                    auto wallNode = scene2::PolygonNode::alloc(wallRect);
                    wallNode->setAnchor(Vec2::ANCHOR_BOTTOM_LEFT);
                    wallNode->setColor(Color4f::BLUE);

                    wallNode->setPosition(wallPos);
                    wallNode->setPriority(constants::Priority::Room);
                    wallNode->setVisible(false);
                    topRoot->addChild(wallNode);
                    _wallNodes.push_back(wallRect);
                    break;

                }
            }
        case constants::DoorDirection::West:
            {
                if (door) {
                    auto wallDimensions = Size(80, 480);
                    Vec2 wallPos = Vec2(0, -80).add(_origin);
                    auto wallRect = Rect(wallPos, wallDimensions);
                    auto wallNode = scene2::PolygonNode::alloc(wallRect);
                    wallNode->setAnchor(Vec2::ANCHOR_BOTTOM_LEFT);
                    wallNode->setColor(Color4f::BLUE);

                    wallNode->setPosition(wallPos);
                    wallNode->setPriority(constants::Priority::Room);
                    wallNode->setVisible(false);
                    topRoot->addChild(wallNode);
                    _wallNodes.push_back(wallRect);

                    Vec2 wallPos2 = Vec2(0, constants::ROOM_DIMENSIONS.height / 2 + 80).add(_origin);
                    auto wallRect2 = Rect(wallPos2, wallDimensions);
                    auto wallNode2 = scene2::PolygonNode::alloc(wallRect2);
                    wallNode2->setAnchor(Vec2::ANCHOR_BOTTOM_LEFT);
                    wallNode2->setColor(Color4f::BLUE);

                    wallNode2->setPosition(wallPos2);
                    wallNode2->setPriority(constants::Priority::Room);
                    wallNode2->setVisible(false);
                    topRoot->addChild(wallNode2);
                    _wallNodes.push_back(wallRect2);
                    break;
                }
                else {
                    //leftmost wall
                    auto wallDimensions = Size(80, 1120);
                    Vec2 wallPos = Vec2(0, 0).add(_origin);
                    auto wallRect = Rect(wallPos, wallDimensions);
                    auto wallNode = scene2::PolygonNode::alloc(wallRect);
                    wallNode->setAnchor(Vec2::ANCHOR_BOTTOM_LEFT);
                    wallNode->setColor(Color4f::BLUE);

                    wallNode->setPosition(wallPos);
                    wallNode->setPriority(constants::Priority::Room);
                    wallNode->setVisible(false);
                    topRoot->addChild(wallNode);
                    _wallNodes.push_back(wallRect);
                    break;
                }
            }
        }
        ++dir;
    }

}