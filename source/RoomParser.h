#pragma once

#include <cugl/cugl.h>
using namespace std;
using namespace cugl;

/**
* This class represents the room parser. It's responsible for
* taking in json representations of rooms and creating room objects
* as per the json's specifications
*/


/** Metadata representing a single obstacle */
struct ObsMetadata {
    string name; // the name used to reference the texture
    bool flip; // whether or not to flip the asset
    Vec2 position; // the position
    Vec2 hitbox; // the hitbox dimensions

    ObsMetadata(string name, bool flip, Vec2 position, Vec2 hitbox) {
        this->name = name;
        this->flip = flip;
        this->position = position;
        this->hitbox = hitbox;
    }
};

/** Metadata representing the layout for a room */
struct LayoutMetadata {
    vector<ObsMetadata> obstacles;
    vector<Vec2> spawns; // where batteries can spawn

    LayoutMetadata(vector<ObsMetadata> obstacles, vector<Vec2> spawns) {
        this->obstacles = obstacles;
        this->spawns = spawns;
    }
};

/** Metadata representing the room before obstacles have been added
 *  This is used for creating a map 
 */
struct RoomMetadata {
    vector<bool> doors;
    Vec2 rank;

    RoomMetadata(vector<bool> doors, Vec2 rank) {
        this->doors = doors;
        this->rank = rank;
    }
};

/** Metadata representing a map, contains information about the rooms, 
 *  where the start and end room are, and how many batteries to spawn
 */
struct MapMetadata {
    vector<RoomMetadata> rooms;
    Vec2 start;
    Vec2 end;
    int numBatteries;

    MapMetadata(vector<RoomMetadata> rooms, Vec2 start, Vec2 end, int numBatteries) {
        this->rooms = rooms;
        this->start = start;
        this->end = end;
        this->numBatteries = numBatteries;
    }
};

class RoomParser {
 
private:
    template <typename T>

    /** Returns the index to a random element of a list of floats */
    int getRandIndex(vector<T> lst);

public:
    shared_ptr<AssetManager> _assets;
    shared_ptr<scene2::OrderedNode> _node;

    /** Creates a new Parser with the default values */
    RoomParser() {};

    /** Releases all resources allocated with this Parser */
    ~RoomParser() { dispose(); };

    /** Initializes a new Parser */
    bool init() {
        return true;
    };

    void dispose() {
        _assets = nullptr;
        _node = nullptr;
    }

    /** Takes in a door configuration and returns the path to an obstacle layout */
    string pickLayout(string code);

    /** Returns the path to a random map layout */
    string pickMap();

    /** Takes in a json file and returns the room layout */
    shared_ptr<LayoutMetadata>getLayoutData(string file);

    /** Takes in a json file and returns the corresponding map.
     *  Also picks a start room and end room combination.
     */
    shared_ptr<MapMetadata> getMapData(string file);

    /** Constructs an array of ints that represents the map metadata for networking purposes
     *  The first element is a list that represents the door configurations. Each room is represented
     *  by 6 ints. The first 2 ints represent the ranking of the room, and the last 4 represent whether
     *  the corresponding cardinal direction has a door
     * 
     *  [x value, y value, north, south, east, west, x value, y value, north, south, east, west, ...]
     *  
     *  The second list represents which obstacle layout is used by that room. The two lists should be ordered
     *  the same, so the first element of this second list represents the layout of the room described by the 
     *  first six elements of the previous list.
     */
    vector<vector<int>> makeNetworkMap(MapMetadata map);
};
