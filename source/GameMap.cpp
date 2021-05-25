#include "GameMap.h"

#define BATTERY_RADIUS 40 

using namespace std;
using namespace cugl;

#pragma mark Constructors
bool GameMap::init(const shared_ptr<AssetManager>& assets, shared_ptr<scene2::OrderedNode>& node) {
    _assets = assets;
    _node = node;
    _teleCount = 4;
    return true;
}

#pragma mark Gameplay Handling

/** Adds a trap to _traps, delete after traps properly implemented */
void GameMap::addTrap(Vec2 pos) {
    auto litRoot = _node->getChildByName("lit_root");
    auto topRoot = _node->getChildByName("top_root");
    auto trap = Trap::alloc(_player->getLoc());
    trap->setArmed();
    auto trapNode = scene2::PolygonNode::allocWithTexture(_assets->get<Texture>("ghost_shadow_texture"));
    trapNode->setAnchor(Vec2::ANCHOR_CENTER);
    //trapNode->setScale(0.25f); // TEMP
    trapNode->setPosition(pos);
    litRoot->addChild(trapNode);

    auto chandelierNode = scene2::AnimationNode::alloc(_assets->get<Texture>("chandelier_texture"), 1, 19);
    chandelierNode->setAnchor(Vec2::ANCHOR_BOTTOM_CENTER);
    chandelierNode->setPriority(constants::Priority::Ceiling);
    chandelierNode->setPosition(pos + constants::TRAP_CHANDELIER_OFFSET);
    topRoot->addChild(chandelierNode);

    auto smokeNode = scene2::AnimationNode::alloc(_assets->get<Texture>("chandelier_smoke_texture"), 1, 8);
    smokeNode->setAnchor(Vec2::ANCHOR_BOTTOM_CENTER);
    smokeNode->setPriority(constants::Priority::Ceiling);
    smokeNode->setPosition(pos + constants::TRAP_SMOKE_OFFSET);
    topRoot->addChild(smokeNode);

    auto trapRadiusNode = scene2::PolygonNode::allocWithTexture(_assets->get<Texture>("trap_radius"));
    trapRadiusNode->setVisible(false);
    trapRadiusNode->setScale(0.5);
    trapRadiusNode->setPosition(Vec2(trapNode->getWidth(), trapNode->getHeight()) * 0.5); // 1/2 * 4 because of 0.25 scale
    trapNode->addChildWithName(trapRadiusNode, "radius");

    trap->setNode(trapNode, chandelierNode, smokeNode);
    trap->setLoc(pos);
    _traps.push_back(trap);
}

/** Method to update the GameMap model, called by GameScene */
void GameMap::update(float timestep) {
    vector<shared_ptr<Trap>> newTraps;
    for (auto& t : _traps) {
        t->update(timestep);
        if (t->justTriggered()) {
            for (auto& p : _players) {
                if (p->getType() == constants::PlayerType::Pal) {
                    if ((t->getLoc() - p->getLoc()).length() < TRAP_RADIUS) {
                        auto pal = dynamic_pointer_cast<Pal>(p);
                        if (!pal->getSpooked()) {
                            pal->setSpookFlag();
                        }
                    }
                }
            }
        }
        if (!t->doneTriggering()) {
            newTraps.push_back(t);
        }
    }
    _traps = newTraps;
    for (auto& s : _slots) {
        s->update(timestep);
    }


    //pal battery interactions
    for (auto& p : _players) {
        if (p->getType() == constants::PlayerType::Pal) {
            auto pal = dynamic_pointer_cast<Pal>(p);
            if (pal->getBatteries() < constants::MAX_BATTERIES) {
                for (auto& b : _batteries) {
                    Vec2 distance = b->getLoc() - p->getLoc();
                    float minDistance = distance.length();
                    if (minDistance < BATTERY_RADIUS && !b->isDestroyed()) {
                        b->pickUp();
                        int numBatteries = pal->getBatteries() + 1;
                        pal->setBatteries(numBatteries);
                    }
                }
            }
        }
    }

    vector<shared_ptr<Battery>> newBatteries;
    for (auto& b : _batteries) {
        if (!b->isDestroyed()) {
            newBatteries.push_back(b);
        }
    }
    _batteries = newBatteries;

    _player->update(timestep);

    // If ghost is tagged, lower the tag timer
    /*
     int tagTimer = _ghostModel->getTimer();
     if (tagTimer > 0) {
     _ghostModel->setTimer(tagTimer - 1);
     }
     if (_ghostModel->getTimer() == 0) {
     _ghostModel->setTagged(false);
     }*/

     // Call GameRoom::update(timestep)

     // Check victory conditions
}

/** Method to update player velocity and players */
void GameMap::move(Vec2 move, Vec2 direction) {
    Vec2 velocity = _player->predictVelocity(move);
    Vec2 loc = _player->getLoc() + velocity + Vec2(-20, -10);
    
    Rect hitbox = Rect(loc, constants::PLAYER_HITBOX_DIMENSIONS);
    
    for (auto& room : getRooms()) {
        if (_player->getType() == constants::PlayerType::Pal) {
            //obstacle collision here
        }
        for (auto& wall : room->getWalls()) {
            if (hitbox.doesIntersect(wall)) {
                _player->updateVelocity(Vec2::ZERO);
                _player->setDir(move);
                return;
            }
        }
    }
    _player->updateVelocity(move);
    if (_player->getType() == constants::PlayerType::Pal) {
        // if pal is helping, freeze the vision cone direction
        if (direction != Vec2::ZERO && !dynamic_pointer_cast<Pal>(_player)->getHelping()) {
            _player->setDir(direction);
        }
    }
    else {
        if (move != Vec2::ZERO) {
            _player->setDir(move);
        }
    }
}

/** Helper method to handle the "interact" input from the players */
void GameMap::handleInteract() {
    float range = 250.0f;
    if (_player->getType() == constants::PlayerType::Pal && !dynamic_pointer_cast<Pal>(_player)->getSpooked()) {
        shared_ptr<Pal> pal;
        float minDistance = numeric_limits<float>::infinity();
        for (auto& p : _players) {
            if (p->getType() == constants::PlayerType::Pal && p != _player && dynamic_pointer_cast<Pal>(p)->getSpooked()) {
                Vec2 distance = p->getLoc() - _player->getLoc();
                if (distance.length() < minDistance) {
                    pal = dynamic_pointer_cast<Pal>(p);
                    minDistance = distance.length();
                }
            }
        }
        if (minDistance < range) {
            dynamic_pointer_cast<Pal>(_player)->setHelping();
            dynamic_pointer_cast<Pal>(pal)->setUnspookFlag();
            return;
        }

        pal = dynamic_pointer_cast<Pal>(_player);

        shared_ptr<BatterySlot> slot;
        minDistance = numeric_limits<float>::infinity();
        for (auto& room : _rooms) {
            auto s = room->getSlot();
            if (s == nullptr) continue;
            Vec2 distance = s->getLoc() - _player->getLoc();
            if (distance.length() < minDistance) {
                slot = s;
                minDistance = distance.length();
            }
        }
        if (slot != nullptr && minDistance < SLOT_RADIUS) {
            if (slot->getCharge() <= 0 && !slot->activated() && pal->getBatteries() > 0) {
                slot->activate();
                pal->setBatteries(pal->getBatteries() - 1);
            }
        }

        // Check if pal is in range of teleporter
        Vec2 tpPos = _endRank * constants::WALL_LENGTH + constants::TELEPORTER_POS;
        Vec2 distance = tpPos - _player->getLoc();
        if (distance.length() < range && pal->getBatteries() > 0) {
            _teleCount -= 1;
            pal->setBatteries(pal->getBatteries() - 1);
        }

    }
    else if (_player->getType() == constants::PlayerType::Ghost) {
        shared_ptr<Trap> trap;
        
        float minDistance = numeric_limits<float>::infinity();
        for (auto& t : _traps) {
            if (!t->getTriggered()) {
                Vec2 distance = t->getLoc() - _player->getLoc();
                if (distance.length() < minDistance) {
                    trap = t;
                    minDistance = distance.length();
                }
            }
        }
        if (trap == nullptr) {
            addTrap(_player->getLoc());
        }
        else {
            if (minDistance < TRAP_RADIUS) {
                
                trap->setTriggered();
                if (trap->justTriggered()) {
                    dynamic_pointer_cast<Ghost>(_player)->setSpooking(true);
                }
                
            }
            else {
                addTrap(_player->getLoc());
            }
        }
        
    }
}

#pragma mark -

bool GameMap::assertValidMap() {
    // make sure there are no overlapping rooms
    set<Vec2> origins;
    for (vector<shared_ptr<GameRoom>>::iterator roomIt = _rooms.begin(); roomIt != _rooms.end(); ++roomIt) {
        shared_ptr<GameRoom> room = *roomIt;
        set<Vec2>::iterator it = origins.find(room->getOrigin());
        if (it != origins.end()) return false;
        origins.insert(room->getOrigin());
    }
    
    return true;
};

void GameMap::makeNodes() {
    auto litRoot = _node->getChildByName("lit_root");

    // Rooms
    for (auto& room : _rooms) {
        auto node = scene2::OrderedNode::allocWithOrder(scene2::OrderedNode::Order::ASCEND);
        auto origin = room->getOrigin();
        node->setContentSize(constants::ROOM_DIMENSIONS);
        node->doLayout();
        node->setName("room_" + to_string(room->getLayout()));
        litRoot->addChild(node);
        room->setNode(node);
        room->setRoot(_node);
        room->addObstacles();
    }

    // Batteries
    for (auto& battery : _batteries) {
        auto batteryNode = scene2::PolygonNode::allocWithTexture(_assets->get<Texture>("battery_texture"));
        batteryNode->setAnchor(Vec2::ANCHOR_BOTTOM_CENTER);
        batteryNode->setPriority(constants::RoomEntity);
        Vec2 coord = battery->getLoc();
        batteryNode->setPosition(coord);
        litRoot->addChild(batteryNode);
        battery->setNode(batteryNode);
    }
}

bool GameMap::generateRandomMap() {
    reset();

    shared_ptr<RoomParser> parser = make_shared<RoomParser>();
    _mapData = parser->getMapData(parser->pickMap());

    _startRank = _mapData->start;
    _endRank = _mapData->end;

    int i = 0;
    int type = 0;
    for (auto& room : _mapData->rooms) {
        shared_ptr<GameRoom> r = GameRoom::alloc(_assets, room.doors, room.rank);
        if (room.rank == _endRank) {
            r->setWinRoom(true);
            type = 1;
        }
        else if (room.rank == _startRank) {
            type = 2;
        }
        r->pickLayout(parser, type);

        for (auto& coord : r->getBatterySpawns()) {
            // Adjust the coordinates of this room's battery spawns
            Vec2 finalCoord = (coord * constants::TILE_SIZE) + Vec2(80, 0) + r->getOrigin();
            _batteriesSpawnable.push_back(finalCoord);
        }

        addSlot(r->getSlot());
        _rooms.push_back(r);
        type = 0;
    }

    // Pick random batteries

    static auto rng = default_random_engine{};
    shuffle(_batteriesSpawnable.begin(), _batteriesSpawnable.end(), rng);
    for (int i = 0; i < _mapData->numBatteries; i++) {
        // This node stuff should be moved to battery
        Vec2 coord = _batteriesSpawnable.back();
        auto batteryModel = Battery::alloc(coord);
        _batteries.push_back(batteryModel);
        _batteriesSpawnable.pop_back();
    }

    // DELETE THIS ONCE NETWORKING IS IMPLEMENTED
    makeNodes();
    return true;   
    
}


Vec2 GameMap::getWinRoomOrigin() {
    for (shared_ptr r : _rooms) {
        if (r->getWinRoom()) {
            return r->getOrigin();
        }
    }
    // default
    return _rooms[8]->getOrigin();
}


/** Constructs metadata to send over the network for map generation */
shared_ptr<MapNetworkdata> GameMap::makeNetworkMap() {
    vector<shared_ptr<RoomNetworkdata>> rooms;
    for (auto& room : _rooms) {
        rooms.push_back(make_shared<RoomNetworkdata>(room->getDoors(), room->getRanking(), room->getLayout()));
    }

    vector<Vec2> batteries;
    for (auto& battery : _batteries) {
        batteries.push_back(battery->getLoc());
    }
    return make_shared<MapNetworkdata>(rooms, batteries, _startRank, _endRank);
}

/** Takes in the network metadata and updates the GameMap model */
bool GameMap::readNetworkMap(shared_ptr<MapNetworkdata> networkData) {
    _rooms.clear();
    _batteries.clear();
    _startRank = networkData->startRank;
    _endRank = networkData->endRank;

    for (auto& room : networkData->rooms) {
        _rooms.push_back(GameRoom::alloc(_assets, room->doors, room->rank, room->layout));
    }
    for (auto& coord : networkData->batteries) {
        auto batteryModel = Battery::alloc(coord);
        _batteries.push_back(batteryModel);
    }
    return true;;
}