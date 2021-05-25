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

#define SLOT_RADIUS 500
#define TRAP_RADIUS 120 // temp, should be 500px

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
    Vec2 startRank;
    Vec2 endRank;

    MapNetworkdata(vector<shared_ptr<RoomNetworkdata>> rooms, vector<Vec2> batteries, Vec2 startRank, Vec2 endRank) {
        this->rooms = rooms;
        this->batteries = batteries;
        this->startRank = startRank;
        this->endRank = endRank;
    }
};

/** Class modeling a game map. */
class GameMap {
private:
    shared_ptr<AssetManager> _assets;

    shared_ptr<scene2::OrderedNode> _node;
    
    shared_ptr<scene2::OrderedNode> litRoot;
    shared_ptr<scene2::OrderedNode> dimRoot;
    shared_ptr<scene2::OrderedNode> topRoot;
    
    
    /** All the rooms in the map */
    vector<shared_ptr<GameRoom>> _rooms;
    
    /** Models representing the players */
    shared_ptr<Player> _player;
    vector<shared_ptr<Player>> _players;
    
    /** The vector of trap pointers currently in the game */
    vector<shared_ptr<Trap>> _traps;
    
    /** The vector of slot pointers currently in the game */
    vector<shared_ptr<BatterySlot>> _slots;

    /** How many batteries the teleporter needs to be powered */
    int _teleCount;

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
    
    bool init(const shared_ptr<AssetManager>& assets);
    
    /**
     * Disposes of all (non-static) resources allocated to this mode.
     */
    virtual void dispose() {
        _assets = nullptr;
        _player = nullptr;
        
        litRoot = nullptr;
        dimRoot = nullptr;
        topRoot = nullptr;
        
        _players.clear();
        _rooms.clear();
        _traps.clear();
        _slots.clear();
        _batteries.clear();
    };

    /**
     * Allocates this model
     */
    static shared_ptr<GameMap> alloc(shared_ptr<AssetManager>& assets) {
        shared_ptr<GameMap> result = make_shared<GameMap>();
        return (result->init(assets) ? result : nullptr);
    }

    void setRoot(shared_ptr<scene2::OrderedNode>& lit, shared_ptr<scene2::OrderedNode>& dim, shared_ptr<scene2::OrderedNode>& top) {
        litRoot = lit;
        dimRoot = dim;
        topRoot = top;
    }
#pragma mark -
#pragma mark State Access
    
    /** Returns the list of rooms */
    vector<shared_ptr<GameRoom>> getRooms() { return _rooms; }
    
    /** Removes references for all rooms */
    void reset() { _rooms.clear(); }
    
    /** Returns the list of traps, delete after traps properly implemented */
    vector<shared_ptr<Trap>> getTraps() { return _traps; }

    /** Sets traps */
    void setTraps(const vector<Vec2>& trapPositions) {
        vector<shared_ptr<Trap>> newTraps;
        vector<Vec2> newTrapPositions;
        for (auto& pos : trapPositions) {
            bool trapExists = false;
            for (auto& trap : _traps) {
                if (trap->getLoc().distance(pos) < TRAP_RADIUS && find(newTraps.begin(), newTraps.end(), trap) == newTraps.end()) {
                    trapExists = true;
                    newTraps.push_back(trap);
                    break;
                }
            }

            if (!trapExists) {
                newTrapPositions.push_back(pos);
            }
        }
        _traps = newTraps;
        for (auto& pos : newTrapPositions) {
            addTrap(pos);
        }
    }
    
    /** @return the player model */
    shared_ptr<Player> getPlayer() { return _player; }

    /** Returns the list of players */
    vector<shared_ptr<Player>> getPlayers() { return _players; }

    /** Returns the amount of batteries that need to be added to the teleporter for the pals to win */
    int getTeleCount() { return _teleCount; }

    void setTeleCount(int num) { _teleCount = num; }

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
    
    /** Generates a random map */
    bool generateRandomMap();

    bool readNetworkMap(shared_ptr<MapNetworkdata> networkData);

    void makeNodes();

    /** Constructs metadata to send over the network for map generation */
    shared_ptr<MapNetworkdata> makeNetworkMap();

#pragma mark -
};

#endif /** __GAME_MAP_H__ */
