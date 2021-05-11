#pragma once
/**

This class is the superclass which all other RoomEntities subclass. It contains their texture, and position, and a node
for animations

*/

#include <cugl/cugl.h>
using namespace std;
using namespace cugl;

class RoomEntity {

protected:
    /** Location of the entity */
    Vec2 _loc;

    /** Reference to the entity's sprite for drawing */
    shared_ptr<scene2::PolygonNode> _node;

public:

    /**
    Returns the position of the entity
    */
    Vec2 getLoc() const {
        return _loc;
    }

    /** 
    Returns the entity's animation node
    */
    const shared_ptr<scene2::PolygonNode> getNode() const {
        return _node;
    }

    /**
    Sets the position of the entity
    */
    void setLoc(Vec2 position) {
        _loc = position;
        if (_node != nullptr) {
            _node->setPosition(_loc);
        }
    }

    /**
    Sets the entity's animation node
    */
    void setNode(const shared_ptr<scene2::AnimationNode> node) {
        _node = node;
    }

    /** Creates a RoomEntity with the default values */
    RoomEntity() {};
    
    ~RoomEntity() {
        dispose();
    }

    /**
    * Initializes a new GameEnity at the origin with radius 0
    * @return true if the GameEntity is initialized properly, false otherwise
    */
    virtual bool init() { return init(cugl::Vec2::ZERO); }

    /** Initializes a new GameEntity at given location */
    virtual bool init(const cugl::Vec2& loc);

    /** Releases all resources allocated with this entity */
    void dispose() {
        _node = nullptr;
    };
    
    static std::shared_ptr<RoomEntity> alloc() {
        std::shared_ptr<RoomEntity> result = std::make_shared<RoomEntity>();
        return (result->init() ? result : nullptr);
    }

    /**
    * Returns a newly allocated Pal at the given position
    * @param pos Initial position in world coordinates
    *
    * @return a newly allocated Pal at the given position
    */
    static std::shared_ptr<RoomEntity> alloc(const cugl::Vec2& loc) {
        std::shared_ptr<RoomEntity> result = std::make_shared<RoomEntity>();
        return (result->init(loc) ? result : nullptr);
    }
    
    void update(float timestep);
    
    void reset();
};
