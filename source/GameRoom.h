#pragma once

#ifndef __GAME_ROOM_H__
#define __GAME_ROOM_H__
#include <cugl/cugl.h>
#include "RoomEntities/BatterySlot.h"
#include "RoomParser.h"
#include "Constants.h"


constexpr int ROOM_DIMENSION = 960;

using namespace std;
using namespace cugl;

/** Class modeling a game room. 
 * GameScene needs to be able to get information from GameRoom in order
 * to create the Box2d objects.
 *
 * GameMap needs to be able to change whether to draw a filled in or empty battery slot
 */
class GameRoom {
private:
    shared_ptr<AssetManager> _assets;

    /** The node containing the nodes for the floor + wall, slot, cable,
     * and obstacles.
     */
    shared_ptr<scene2::OrderedNode> _node;

    /** Root node for GameScene */
    shared_ptr<scene2::OrderedNode> _root;
    
    /** The nodes for the floor + wall, slot, and cable */
    // These should only be alloc'd with position, as textures will be set in GameScene
    shared_ptr<scene2::PolygonNode> _roomNode;
    shared_ptr<BatterySlot> _slotModel;
    shared_ptr<scene2::PolygonNode> _cableNode;
    
    /** The origin of the room. Distance from (0,0) of the map to room's bottom left corner */
    Vec2 _origin;
    
    /** Where in the grid of rooms this room is */
    Vec2 _ranking;

    /** If this room has layouts, delete after all rooms have layouts */
    bool _layout;

    bool _winRoom;
    
    /** Possible locations within the room where batteries can spawn */
    vector<vector<int>> _batterySpawns;
    
    // List of bools that represent which side has a door and which doesn't. In order of North, East, South, and West
    vector<bool> _doors;
    
    bool assertValidRoom();
    
    bool assertRoomIsAdjacent(const shared_ptr<GameRoom>& room);
    
public:
    GameRoom() {}

    ~GameRoom() { }

    bool init(const shared_ptr<AssetManager>& assets, const shared_ptr<scene2::OrderedNode>& node, const Vec2& origin, vector<bool> doors, Vec2 ranking);

    bool init(const shared_ptr<AssetManager>& assets, const Vec2& origin, vector<bool> doors, Vec2 ranking);

    static shared_ptr<GameRoom> alloc(const shared_ptr<AssetManager>& assets, const shared_ptr<scene2::OrderedNode>& node, const Vec2& origin, vector<bool> doors, Vec2 ranking) {
        shared_ptr<GameRoom> result = make_shared<GameRoom>();
        return (result->init(assets, node, origin, doors, ranking) ? result : nullptr);
    }
    
    static shared_ptr<GameRoom> alloc(const shared_ptr<AssetManager>& assets, const Vec2& origin, vector<bool> doors, Vec2 ranking) {
        shared_ptr<GameRoom> result = make_shared<GameRoom>();
        return (result->init(assets, origin, doors, ranking) ? result : nullptr);
    }

    // Gets the coordinates of the origin of this room
    Vec2 getOrigin() { return _origin; }

    // Gets the ranking of the room
    Vec2 getRanking() { return _ranking; }

    // Gets the scene node
    shared_ptr<scene2::OrderedNode> getNode() { return _node; };

    /**
     * @param value The Player node.
     */
    void setNode(const shared_ptr<scene2::OrderedNode>& value);

    /**
     * @param value The Player node.
     */
    void setRoot(const shared_ptr<scene2::OrderedNode>& value);

    // Gets the list of battery spawns
    vector<vector<int>> getBatterySpawns() { return _batterySpawns; };

    // Gets the list of doors
    vector<bool> getDoors() { return _doors; };

    // Gets the list of doors in string form
    string getDoorsStr() {
        string outp = "";
        for (bool d : _doors) {
            if (d) {
                outp += "1";
            }
            else {
                outp += "0";
            }
        }
        return outp;
    }
    
    shared_ptr<BatterySlot> getSlot() { return _slotModel; };
    
    bool getLight() { return _slotModel->getCharge() > 0;};

    /** Populates the room with obstacles from the parser. If end is true,
     *  uses end.json's layout for obstacles 
     */
    void addObstacles(const shared_ptr<RoomParser>& parser, bool end);
    
    /** Is this room the exit room? */
    void setWinRoom(bool exit) { _winRoom = exit; };
    bool getWinRoom() { return _winRoom; };
    
    /** Returns the layout of the room */
    bool getLayout() { return _layout; };

    // Comparison operator
    friend bool operator<(const shared_ptr<GameRoom>& l, const shared_ptr<GameRoom>& r) {
        if (l->_origin.x != r->_origin.x) {
            return l->_origin.x < r->_origin.x;
        }
        else if (l->_origin.y != r->_origin.y) {
            return l->_origin.y < r->_origin.y;
        }
        return false;
    }
    
};

#endif /** __GAME_ROOM_H__ */
