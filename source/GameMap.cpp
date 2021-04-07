#include "GameMap.h"
#include "RoomParser.h"

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

bool GameMap::setTextures(vector<shared_ptr<Texture>> textures) {
	shared_ptr<Texture> floor_texture = textures[0];
	shared_ptr<Texture> door_texture = textures[1];
	for (auto& room : _rooms) {
		shared_ptr<scene2::PolygonNode> node = scene2::PolygonNode::allocWithTexture(floor_texture);
		node->setPosition(room->getOrigin());
		node->addChild(scene2::PolygonNode::allocWithTexture(door_texture));
		_nodes.push_back(node);
	}
	return true;
}