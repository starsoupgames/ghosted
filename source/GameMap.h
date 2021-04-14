#pragma once

#ifndef __GAME_MAP_H__
#define __GAME_MAP_H__
#include <cugl/cugl.h>
#include "GameRoom.h"
#include "GameEntities/Players/PlayerPal.h"
#include "GameEntities/Players/PlayerGhost.h"
#include "RoomEntities/Trap.h"
#include "RoomEntities/BatterySlot.h"

using namespace std;
using namespace cugl;

/** Class modeling a game map. */
class GameMap {
private:

	/** The Box2D world */
	shared_ptr<physics2::ObstacleWorld> _world;

	/** All the rooms in the map */
	vector<shared_ptr<GameRoom>> _rooms;

	/** Models representing the players */
	shared_ptr<Player> _player;
	vector<shared_ptr<Player>> _otherPlayers;
	shared_ptr<Pal> _palModel;
	shared_ptr<Ghost> _ghostModel;

	/** The vector of trap pointers currently in the game */
	std::vector<shared_ptr<Trap>> _traps;
	/** The vector of slot pointers currently in the game */
	std::vector<shared_ptr<BatterySlot>> _slots;

	/**
	 * The pair of bools representing if the game is complete
	 * and who won, repectively. True correponds to the Pals winning */
	bool _complete [2];

	bool assertValidMap();

	bool generateBasicMap();

	/** Helper method to handle the "interact" input from the players */
	void handleInteract(shared_ptr<Trap> trap);

public:
#pragma mark Constructors
	GameMap() { }

	~GameMap() { dispose(); }

	bool init();

	/**
	* Disposes of all (non-static) resources allocated to this mode.
	*/
	virtual void dispose();

	static shared_ptr<GameMap> alloc() {
		shared_ptr<GameMap> result = make_shared<GameMap>();
		return (result->init() ? result : nullptr);
	}
#pragma mark -
#pragma mark State Access

	/** Returns the list of rooms */
	vector<shared_ptr<GameRoom>> getRooms() { return _rooms; }

	/** Removes references for all rooms */
	void reset() { _rooms.clear(); }

	/** Returns the reference to the Pal model */
	shared_ptr<Pal> getPal() { return _palModel; }

	/** Returns the reference to the Ghost model */
	shared_ptr<Ghost> getGhost() { return _ghostModel; }

	/** Returns whether or not the game is complete and who won */
	bool* getComplete() { return _complete; }

	/** Gets the list of traps, delete after traps properly implemented */
	vector<shared_ptr<Trap>> getTraps() { return _traps; }

	/** Sets the model for the player */
	void setPlayer(shared_ptr<Player> val) { _player = val; }

	/** Sets the model for other players */
	void setOtherPlayers(vector<shared_ptr<Player>> val) { _otherPlayers = val; }

	/** Adds a charged slot to _slots, delete after batteries properly implemented */
	void addSlot(shared_ptr<BatterySlot> slot) { _slots.push_back(slot); }

	/** Adds a trap to _traps, delete after traps properly implemented */
	void addTrap(shared_ptr<Trap> trap) { _traps.push_back(trap); }

#pragma mark -
#pragma mark Gameplay Handling

	/** Method to update the GameMap model, called by GameScene */
	void update(float timestep, bool interact);

	/** Method to move and change the direction of players */
	void move(Vec2 move, Vec2 direction);

#pragma mark -
#pragma mark Map Gen

	/** Generates a random map */
	bool generateRandomMap();
#pragma mark -

};

#endif /** __GAME_MAP_H__ */