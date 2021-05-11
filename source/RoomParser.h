#pragma once

#include <cugl/cugl.h>
#include "GameRoom.h"
using namespace std;
using namespace cugl;

/**
* This class represents the room parser. It's responsible for
* takin in json representations of rooms and creating room objects
* as per the json's specifications
*/
class RoomParser {
 
public:
    shared_ptr<AssetManager> _assets;
    shared_ptr<scene2::OrderedNode> _node;

    /** Creates a new Parser with the default values */
    RoomParser() {};

    /** Releases all resources allocated with this Parser */
    ~RoomParser() { dispose(); };

    /** Initializes a new Parser */
    bool init(const shared_ptr<AssetManager>& assets, const shared_ptr<scene2::OrderedNode>& node) {
        _assets = assets;
        _node = node;
        return true;
    };

    void dispose() {
        _assets = nullptr;
        _node = nullptr;
    }

    static shared_ptr<RoomParser> alloc(const shared_ptr<AssetManager>& assets, const shared_ptr<scene2::OrderedNode>& node) {
        auto result = make_shared<RoomParser>();
        return (result->init(assets, node) ? result : nullptr);
    }

    /** Takes in a json file and creates a room object */
    shared_ptr<GameRoom> parse(string file, const Vec2& origin);

};
