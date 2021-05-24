#pragma once

#ifndef __GAME_MAP_H__
#define __GAME_MAP_H__
#include <cugl/cugl.h>
#include "GameRoom.h"
#include "GameEntities/Players/PlayerPal.h"
#include "GameEntities/Players/PlayerGhost.h"
#include "GameEntities/BatteryCollectible.h"
#include "GameEntities/Trap.h"
#include "AudioController.h"

using namespace std;
using namespace cugl;

/** Metadata for networking that represents a room. Contains information
 *  about the room's ranking, its door configuration, and which layout it
 *  is using.
 */
struct RoomNetworkdata {
    vector<bool> doors;
    Vec2 rank;
    int layout;

    RoomNetworkdata(vector<bool> doors, Vec2 rank, int layout) {
        this->doors = doors;
        this->rank = rank;
        this->layout = layout;
    }
};

/** Metadata for networking that represents a map. Contains information about the rooms
 *  in the map and the coordinates of batteries that will be spawned in.
 */
struct MapNetworkdata {
    vector<shared_ptr<RoomNetworkdata>> rooms;
    vector<Vec2> batteries;

    MapNetworkdata(vector<shared_ptr<RoomNetworkdata>> rooms, vector<Vec2> batteries) {
        this->rooms = rooms;
        this->batteries = batteries;
    }
};


/** Class modeling a game map. */
class GameMap {
private:
    shared_ptr<AssetManager> _assets;

    shared_ptr<scene2::OrderedNode> _node;
    
    /** All the rooms in the map */
    vector<shared_ptr<GameRoom>> _rooms;
    
    /** Models representing the players */
    shared_ptr<Player> _player;
    vector<shared_ptr<Player>> _players;
    
    /** The vector of trap pointers currently in the game */
    vector<shared_ptr<Trap>> _traps;
    /** The vector of slot pointers currently in the game */
    vector<shared_ptr<BatterySlot>> _slots;

    /** The vector of battery spawnable coordinates */
    vector<Vec2> _batteriesSpawnable;

    /** The vector of batteries */
    vector<shared_ptr<Battery>> _batteries;

    /** The metadata represeting the map */
    shared_ptr<MapMetadata> _mapData;
    
    /** The ranking of the start room */
    Vec2 _startRank;

    /** The ranking of the end room */
    Vec2 _endRank;

    bool assertValidMap();
    

public:
#pragma mark Constructors
    GameMap() { }
    
    ~GameMap() { dispose(); }
    
    bool init(const shared_ptr<AssetManager>& assets, shared_ptr<scene2::OrderedNode>& node);
    
    /**
     * Disposes of all (non-static) resources allocated to this mode.
     */
    virtual void dispose() {
        _assets = nullptr;
        _node = nullptr;
        _player = nullptr;
        _players.clear();
        _rooms.clear();
        _traps.clear();
        _slots.clear();
        _batteries.clear();
    };

    
    /**
     * Allocates this model
     */
    static shared_ptr<GameMap> alloc(shared_ptr<AssetManager>& assets, shared_ptr<scene2::OrderedNode>& node) {
        shared_ptr<GameMap> result = make_shared<GameMap>();
        return (result->init(assets, node) ? result : nullptr);
    }
#pragma mark -
#pragma mark State Access
    
    /** Returns the list of rooms */
    vector<shared_ptr<GameRoom>> getRooms() { return _rooms; }
    
    /** Removes references for all rooms */
    void reset() { _rooms.clear(); }
    
    /** Returns the list of traps, delete after traps properly implemented */
    vector<shared_ptr<Trap>> getTraps() { return _traps; }
    
    /** @return the player model */
    shared_ptr<Player> getPlayer() { return _player; }

    /** Returns the list of players */
    vector<shared_ptr<Player>> getPlayers() { return _players; }

    /** Returns the ghost */
    shared_ptr<Player> getGhost() {
        for (auto& p : _players) {
            if (p->getType() == constants::PlayerType::Ghost) {
                return p;
            }
        }
        return nullptr;
    }

    /** Returns the model associated with this body */
    shared_ptr<Player> getModel(b2Body* body) {
        for (auto& p : _players) {
            if (p.get() == body->GetUserData()) {
                return p;
            }
        }
        return nullptr;
    }

    /** Returns the ranking of the starting room */
    Vec2 getStartRank() {
        return _startRank;
    }

    /** Returns the ranking of the end room */
    Vec2 getEndRank() {
        return _endRank;
    }

    Vec2 getWinRoomOrigin();
    
    /** Sets the player model */
    void setPlayer(shared_ptr<Player>& player) { _player = player; }
    
    /** Sets the models for all players */
    void setPlayers(vector<shared_ptr<Player>> players) { _players = players; }
    
    /** Adds a charged slot to _slots, delete after batteries properly implemented */
    void addSlot(shared_ptr<BatterySlot> slot) { _slots.push_back(slot); }
    
    /** Adds a trap to _traps, delete after traps properly implemented */
    void addTrap(Vec2 pos);
    
#pragma mark -
#pragma mark Gameplay Handling
    
    /** Method to update the GameMap model, called by GameScene */
    void update(float timestep);
    
    /** Method to move and change the direction of players */
    void move(Vec2 move, Vec2 direction);
    
    /** Method to handle the interactions */
    void handleInteract();

    
#pragma mark -
#pragma mark Map Gen
    
    /** Generates a basic map */
    bool generateBasicMap(int numBatteries);
    
    /** Generates a random map */
    bool generateRandomMap();

    bool readNetworkMap(shared_ptr<MapNetworkdata> networkData);
    
#pragma mark -
#pragma mark Networking

    /** Constructs metadata to send over the network for map generation */
    shared_ptr<MapNetworkdata> makeNetworkMap();

#pragma mark -
};

#endif /** __GAME_MAP_H__ */
