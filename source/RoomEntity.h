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
    
    /** Radius of the entity in pixels */
    int _radius;

	/** Reference to the entity's sprite for drawing */
	shared_ptr<scene2::AnimationNode> _animationNode;

public:

	/**
	Returns the position of the entity
	*/
	Vec2 getLoc() const {
		return _loc;
	}
    
    float getRadius() const {
        return _radius;
    }

	/** 
	Returns the entity's animation node
	*/
	const shared_ptr<scene2::AnimationNode> getNode() const {
		return _animationNode;
	}

	/**
	Sets the position of the entity
	*/
	void setLoc(Vec2 position) {
		_loc = position;
        if (_animationNode != nullptr) {
            _animationNode->setPosition(_loc);
        }
	}

	/**
	Sets the entity's animation node
	*/
	void setNode(const shared_ptr<scene2::AnimationNode> node) {
		_animationNode = node;
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
    bool init() { return init(cugl::Vec2::ZERO, 0); }

    /** Initializes a new GameEntity at given location */
    bool init(const cugl::Vec2& loc, const int radius);

	/** Releases all resources allocated with this entity */
	void dispose();
    
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
    static std::shared_ptr<RoomEntity> alloc(const cugl::Vec2& loc, const int radius) {
        std::shared_ptr<RoomEntity> result = std::make_shared<RoomEntity>();
        return (result->init(loc, radius) ? result : nullptr);
    }
    
    void update(float timestep);
    
    void reset();
};
