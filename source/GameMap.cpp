#include "GameMap.h"

#define TRAP_RADIUS 120 // temp, should be 500px
#define BATTERY_RADIUS 40 

using namespace std;
using namespace cugl;

#pragma mark Constructors
bool GameMap::init(const shared_ptr<AssetManager>& assets, shared_ptr<scene2::OrderedNode>& node, vector<Vec2> batterySpawnable) {
    _assets = assets;
    _node = node;
    _batteriesSpawnable = batterySpawnable;
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

    auto trapRadiusNode = scene2::PolygonNode::allocWithTexture(_assets->get<Texture>("trap_radius"));
    trapRadiusNode->setVisible(false);
    trapRadiusNode->setScale(0.5);
    trapRadiusNode->setPosition(Vec2(trapNode->getWidth(), trapNode->getHeight()) * 0.5); // 1/2 * 4 because of 0.25 scale
    trapNode->addChildWithName(trapRadiusNode, "radius");

    trap->setNode(trapNode, chandelierNode);
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
                            pal->setSpooked(true);
                        }
                    }
                }
            }
        }
        if (!t->doneTriggering()) {
            newTraps.push_back(t);
        }
    }
    _traps.clear();
    _traps = newTraps;
    for (auto& s : _slots) {
        s->update(timestep);
    }


    //pal battery interactions
    for (auto& p : _players) {
        if (p->getType() == constants::PlayerType::Pal) {
            for (auto& b : _batteries) {
                Vec2 distance = b->getLoc() - p->getLoc();
                float minDistance = distance.length();
                if (minDistance < BATTERY_RADIUS && !b->isDestroyed()) {
                    b->pickUp();
                    auto pal = dynamic_pointer_cast<Pal>(p);
                    int numBatteries = pal->getBatteries() + 1;
                    pal->setBatteries(numBatteries);
                    //CULog("%i", _player->getBatteries());
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
    _player->updateVelocity(move);
    if (_player->getType() == constants::PlayerType::Pal) {
        if (direction != Vec2::ZERO) {
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
    // TODO use position, not node position
    if (_player->getType() == constants::PlayerType::Pal && !dynamic_pointer_cast<Pal>(_player)->getSpooked()) {
        //  CODE FOR WHEN BATTERIES FULLY IMPLEMENTED
        /**
         * if (pal is close to the slot in their room):
         *	slot->setCharge();
         *	slot->getNode()->setColor(Color4f::GREEN);
         *   _slots.push_back(slot);
         */
        shared_ptr<BatterySlot> slot = nullptr;
        
        // TODO fix this
        for (auto room = _rooms.begin(); room != _rooms.end(); ++room) {
            Vec2 slotPos = (*room)->getOrigin() + (*room)->getSlot()->getNode()->getPosition();
            Vec2 distance = slotPos - _player->getNode()->getPosition();
            if (distance.length() < range) {
                slot = (*room)->getSlot();
            }
        }
        if (slot != nullptr) {
            if (slot->getCharge() <= 0) {
                slot->setCharge();
                slot->getNode()->setColor(Color4f::GREEN);
            }
        }

    }
    else if (_player->getType() == constants::PlayerType::Ghost) {
        shared_ptr<Trap> trap = nullptr;
        
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

bool GameMap::generateBasicMap(int numBatteries) {
    reset();
    auto litRoot = _node->getChildByName("lit_root");
    /**
    * 1. Create hard coded model that represent what type of rooms need to be where, no obstacles
    * 2. Assign each room an ostacle layout
    * 3. Call makeRoom() on each room+obstacle combo and add to _rooms
    */
    int spacing = 1120;
    _rooms.push_back(GameRoom::alloc(_assets, _node, Vec2(0, 0), { true, true, false, false }));
    _rooms.push_back(GameRoom::alloc(_assets, _node, Vec2(spacing, 0), { true, true, false, true }));
    _rooms.push_back(GameRoom::alloc(_assets, _node, Vec2(spacing*2, 0), { true, false, false, true }));
    _rooms.push_back(GameRoom::alloc(_assets, _node, Vec2(0, spacing), { true, true, true, false }));
    _rooms.push_back(GameRoom::alloc(_assets, _node, Vec2(spacing, spacing), { true, true, true, true }));
    _rooms.push_back(GameRoom::alloc(_assets, _node, Vec2(spacing*2, spacing), { true, false, true, true }));
    _rooms.push_back(GameRoom::alloc(_assets, _node, Vec2(0, spacing*2), { false, true, true, false }));
    _rooms.push_back(GameRoom::alloc(_assets, _node, Vec2(spacing, spacing * 2), { false, true, true, true }));
    _rooms.push_back(GameRoom::alloc(_assets, _node, Vec2(spacing*2, spacing * 2), { false, false, true, true }));

    for (auto& coor : _batteriesSpawnable) {
        auto batteryNode = scene2::PolygonNode::allocWithTexture(_assets->get<Texture>("battery_texture"));
        batteryNode->setPriority(constants::RoomEntity);
        batteryNode->setPosition(coor);
        litRoot->addChild(batteryNode);
        auto batteryModel = Battery::alloc(coor);
        batteryModel->setNode(batteryNode);
        _batteries.push_back(batteryModel);
    }

    return true;
};

void makeConnection(shared_ptr<GameRoom> room1, shared_ptr<GameRoom> room2) {
}

/*
shared_ptr<GameRoom> makeRoom() {
    // call gameroom's init and pass in obstacle layout and door configuration
    RoomParser parser = RoomParser();
    return parser.parse("json/room.json", Vec2(0, 0));
}
*/

bool GameMap::generateRandomMap() {
    reset();
    /**
     * 1. Create random model that represent what type of rooms need to be where, no obstacles
     * 2. Assign each room an ostacle layout
     * 3. Call makeRoom() on each room+obstacle combo and add to _rooms
     */
    
    /*
    RoomParser parser = RoomParser();
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            shared_ptr<GameRoom> room = parser.parse("json/room.json", Vec2(i * 375, j * 375));
            
            // set win room on top row
            if (i == 2 && j == 2) {
                room->setWinRoom(true);
            }
            
            _rooms.push_back(room);
        }
    }
    */
    
    return true;
    
    
}

/** Generates a random map with the coordinates of spawnable battery locations */
bool GameMap::generateRandomMap(vector<Vec2> batterySpawns) {
    reset();
    /**
     * 1. Create random model that represent what type of rooms need to be where, no obstacles
     * 2. Assign each room an ostacle layout
     * 3. Call makeRoom() on each room+obstacle combo and add to _rooms
     */


    shared_ptr<RoomParser> parser = RoomParser::alloc(_assets, _node);
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            shared_ptr<GameRoom> room = parser->parse("json/room.json", Vec2(i * 375, j * 375));

            // set win room on top row
            if (i == 2 && j == 2) {
                room->setWinRoom(true);
            }

            _rooms.push_back(room);
        }
    }


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
