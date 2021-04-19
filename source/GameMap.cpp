#include "GameMap.h"
#include "RoomParser.h"

#define CONE_WIDTH 75
#define CONE_LENGTH 200
#define ROOM_SIZE 960

#pragma mark Constructors
bool GameMap::init() {
	/** Initialize pal model */
	_palModel = Pal::alloc(Vec2(50, 100));
	_ghostModel = Ghost::alloc(Vec2(200, 100));
	_complete[0] = false;
	_complete[1] = false;
	return true;
}

/**
 * Disposes of all(non - static) resources allocated
 */
void GameMap::dispose() {
	_player = nullptr;
	_palModel = nullptr;
	_ghostModel = nullptr;
	for (auto& room : _rooms) {
		room = nullptr;
	}
	for (auto& p : _otherPlayers) {
		p = nullptr;
	}
}


#pragma mark Gameplay Handling

/** Method to update the GameMap model, called by GameScene */
void GameMap::update(float timestep) {
	shared_ptr<Trap> trap = nullptr;

	for (shared_ptr s : _slots) {
		s->update(timestep);
	}

	_player->update(timestep);

	// If ghost is tagged, lower the tag timer
	int tagTimer = _ghostModel->getTimer();
	if (tagTimer > 0) {
		_ghostModel->setTimer(tagTimer - 1);
	}
	if (_ghostModel->getTimer() == 0) {
		_ghostModel->setTagged(false);
	}

	// Call GameRoom::update(timestep)

	// Check victory conditions
	if (_palModel->getSpooked()) {
		_complete[0] = true;
		_complete[1] = false;
	}
	else if (_palModel->getBatteries() == 3) {
		_complete[0] = true;
		_complete[1] = true;
	}
}

/** Method to move and change the direction of players */
void GameMap::move(Vec2 move, Vec2 direction) {
	_player->setMove(move);
	_player->setIdle(move == Vec2::ZERO);
	if (direction != Vec2::ZERO) {
		_player->setDir(direction);
	}
}

/** Helper method to handle the "interact" input from the players */
void GameMap::handleInteract(bool pal) {
    float range = 250.0f;
    if (pal) {
		//  CODE FOR WHEN BATTERIES FULLY IMPLEMENTED
		/**
		* if (pal is close to the slot in their room):
		*	slot->setCharge();
		*	slot->getNode()->setColor(Color4f::GREEN);
		*   _slots.push_back(slot);
		*/
        shared_ptr<BatterySlot> slot = nullptr;
        
        for (vector<shared_ptr<GameRoom>>  ::const_iterator room = _rooms.begin();
              room != _rooms.end(); ++room)
            {
                Vec2 slotPos = (*room)->getOrigin() + (*room)->getSlot()->getNode()->getPosition();
                Vec2 distance = slotPos - _player->getNode()->getPosition();
                if (distance.lengthSquared() < range*range) {
                    slot = (*room)->getSlot();
//                    slot->getNode()->setVisible(true);
//                    slot->getNode()->setColor(Color4f::GREEN);
                }
            }
        if (slot != nullptr) {
            if (slot->getCharge() <= 0) {
                slot->setCharge();
                slot->getNode()->setColor(Color4f::GREEN);
            }
        }
	}
	else if (_player->getType() == Player::Type::Ghost) {
		// CODE FOR WHEN TRAPS FULLY IMPLEMENTED
        shared_ptr<Trap> trap = nullptr;
        
        for (vector<shared_ptr<GameRoom>>  ::const_iterator room = _rooms.begin();
              room != _rooms.end(); ++room)
            {
                Vec2 trapPos = (*room)->getOrigin() + (*room)->getTrap()->getNode()->getPosition();
                Vec2 distance = trapPos - _ghostModel->getLoc();
                if (distance.lengthSquared() < range*range) trap = (*room)->getTrap();
        }
        if (trap != nullptr) {
            if (!trap->getTriggered()) {
                trap->setTriggered(true);
                trap->getNode()->setColor(Color4f::RED);
                trap->getNode()->setVisible(true);
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

	/**
	* 1. Create hard coded model that represent what type of rooms need to be where, no obstacles
	* 2. Assign each room an ostacle layout
	* 3. Call makeRoom() on each room+obstacle combo and add to _rooms
	*/
	return true;
};

void makeConnection(shared_ptr<GameRoom> room1, shared_ptr<GameRoom> room2) {
}

shared_ptr<GameRoom> makeRoom() {
	// call gameroom's init and pass in obstacle layout and door configuration
	RoomParser parser = RoomParser();
	return parser.parse("json/room.json", Vec2(0, 0));
}

bool GameMap::generateRandomMap() {
	reset();

	/**
	 * 1. Create random model that represent what type of rooms need to be where, no obstacles
	 * 2. Assign each room an ostacle layout
	 * 3. Call makeRoom() on each room+obstacle combo and add to _rooms
	 */


	RoomParser parser = RoomParser();
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			shared_ptr<GameRoom> room = parser.parse("json/room.json", Vec2(i * 375, j * 375));
			_rooms.push_back(room);
		}
	}
	
	return true;


}
