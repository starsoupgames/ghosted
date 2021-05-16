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

/** Metadata representing a map, contains information about the rooms and
 *  possible spawns
 */
struct MapMetadata {
    vector<RoomMetadata> rooms;
    vector<vector<Vec2>> spawns;

    MapMetadata(vector<RoomMetadata> rooms, vector<vector<Vec2>> spawns) {
        this->rooms = rooms;
        this->spawns = spawns;
    }
};

class RoomParser {
 
private:
    /** Returns the index to a random element of a list of floats */
    int getRandIndex(vector<float> lst);

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

    /** Takes in a json file and returns the corresponding map */
    shared_ptr<MapMetadata> getMapData(string file);
};
