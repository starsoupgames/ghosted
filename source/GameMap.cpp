#include "GameMap.h"
#include "RoomParser.h"

#define CONE_WIDTH 75
#define CONE_LENGTH 200
#define ROOM_SIZE 960

#pragma mark Constructors
bool GameMap::init() {
	/** Initialize pal model */
	_palModel = Pal::alloc(Vec2(-100, 0));
	_ghostModel = Ghost::alloc(Vec2(150, 0));
	_complete[0] = false;
	_complete[1] = false;
	return true;
}

/**
 * Disposes of all(non - static) resources allocated
 */
void GameMap::dispose() {
	_world = nullptr;
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
#pragma mark -
#pragma mark Gameplay Handling

/** Method to update the GameMap model, called by GameScene */
void GameMap::update(float timestep, bool interact) {
	shared_ptr<Trap> trap = nullptr;

	for (shared_ptr s : _slots) {
		s->update(timestep);
	}
	if (interact) {
		//handleInteract(trap);
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

	// Check collisions
	// between players and bounds
	// between pals and walls
	// between ghost and vision cones
	// between ghost and pal
	// between trap and pal if trap is triggered
	// between pal and battery

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
void GameMap::handleInteract(shared_ptr<Trap> trap) {
	if (_player->getType() == Player::Type::Pal && !_palModel->getSpooked()) {
		//  CODE FOR WHEN BATTERIES FULLY IMPLEMENTED
		/**
		* if (pal is close to the slot in their room):
		*	slot->setCharge();
		*	slot->getNode()->setColor(Color4f::GREEN);
		*   _slots.push_back(slot);
		*/
	}
	else if (_player->getType() == Player::Type::Ghost) {
		// CODE FOR WHEN TRAPS FULLY IMPLEMENTED
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
	// make sure connections are two-way
	for (vector<shared_ptr<GameRoom>>::iterator room1It = _rooms.begin(); room1It != _rooms.end(); ++room1It) {
		shared_ptr<GameRoom> room1 = *room1It;
		set<shared_ptr<GameRoom>> connectedRooms1 = room1->getConnectedRooms();
		for (vector<shared_ptr<GameRoom>>::iterator room2It = room1It; room2It != _rooms.end(); ++room2It) {
			shared_ptr<GameRoom> room2 = *room2It;
			if (connectedRooms1.find(room2) != connectedRooms1.end()) {
				set<shared_ptr<GameRoom>> connectedRooms2 = room2->getConnectedRooms();
				if (connectedRooms2.find(room1) == connectedRooms2.end()) return false;
			};
		}
	}
	return true;
};

bool GameMap::generateBasicMap() {
	reset();
	shared_ptr<GameRoom> room1 = GameRoom::alloc(Vec2(0, 0));
	shared_ptr<GameRoom> room2 = GameRoom::alloc(Vec2(600, 0));
	shared_ptr<GameRoom> room3 = GameRoom::alloc(Vec2(1200, 0));
	_rooms.push_back(room1);
	_rooms.push_back(room2);
	_rooms.push_back(room3);
	room1->setConnectedRoom(room2);
	room2->setConnectedRoom(room1);
	room2->setConnectedRoom(room3);
	room3->setConnectedRoom(room2);
	return assertValidMap();
};

void makeConnection(shared_ptr<GameRoom> room1, shared_ptr<GameRoom> room2) {
	room1->setConnectedRoom(room2);
	room2->setConnectedRoom(room1);
}

bool GameMap::generateRandomMap() {
	RoomParser parser = RoomParser();
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			shared_ptr<GameRoom> room = parser.parse("json/room.json", Vec2(i * 375, j * 375));
			_rooms.push_back(room);
		}
	}
	makeConnection(_rooms[0], _rooms[1]);
	makeConnection(_rooms[0], _rooms[3]);
	makeConnection(_rooms[1], _rooms[4]);
	makeConnection(_rooms[1], _rooms[2]);
	makeConnection(_rooms[2], _rooms[5]);
	makeConnection(_rooms[3], _rooms[4]);
	makeConnection(_rooms[3], _rooms[6]);
	makeConnection(_rooms[4], _rooms[7]);
	makeConnection(_rooms[4], _rooms[5]);
	makeConnection(_rooms[5], _rooms[8]);
	makeConnection(_rooms[6], _rooms[7]);
	makeConnection(_rooms[7], _rooms[8]);
	return true;
}
