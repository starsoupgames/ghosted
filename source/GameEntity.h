#pragma once
/**

This class is the superclass which all other GameEntities subclass. It contains their texture, and location, and a node
for animations

*/

#include <cugl/cugl.h>
#include "Constants.h"
using namespace std;
using namespace cugl;

class GameEntity {

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

	/** Returns the Pal's radius */
	int getRadius() const {
		return _radius;
	}

	/**
	Sets the position of the entity
	*/
	void setLoc(Vec2 position) {
		_loc = position;
	}


	/** Creates a GameEntity with the default values */
	GameEntity() {};

	~GameEntity() {
		dispose();
	};

	/**
	* Initializes a new GameEnity at the origin with radius 0
	* @return true if the GameEntity is initialized properly, false otherwise
	*/
	bool init() { return init(cugl::Vec2::ZERO, 0); }

	/** Initializes a new GameEntity at given location */
	bool init(const cugl::Vec2& pos, const int radius);

	/** Releases all resources allocated with this entity */
	void dispose();

	/** Allocator */
	/*
	static std::shared_ptr<GameEntity> alloc(const std::shared_ptr<scene2::SceneNode>& sceneNode,
		Vec2 loc) {
		std::shared_ptr<GameEntity> entity = std::make_shared<GameEntity>();
		return (entity->init(*sceneNode, loc) ? entity : nullptr);
	}
	*/

	static std::shared_ptr<GameEntity> alloc() {
		std::shared_ptr<GameEntity> result = std::make_shared<GameEntity>();
		return (result->init() ? result : nullptr);
	}

	/**
	* Returns a newly allocated Pal at the given position
	* @param pos Initial position in world coordinates
	*
	* @return a newly allocated Pal at the given position
	*/
	static std::shared_ptr<GameEntity> alloc(const cugl::Vec2& pos, const int radius) {
		std::shared_ptr<GameEntity> result = std::make_shared<GameEntity>();
		return (result->init(pos, radius) ? result : nullptr);
	}

#pragma mark -
#pragma mark Animation
	/**
	 * Returns a reference to film strip representing this pal.
	 *
	 * It returns nullptr if there is no active film strip.
	 *
	 * @return a reference to film strip representing this pal.
	 */
	std::shared_ptr<cugl::scene2::AnimationNode>& getSprite() { return _animationNode; }

	/**
	 * Sets the film strip representing this pal.
	 *
	 * Setting this to nullptr clears the value.
	 *
	 * @param value The pal film strip.
	 */
	void setNode(const std::shared_ptr<cugl::scene2::AnimationNode>& value);

	/**
	 * Updates the state of the model
	 *
	 * This method moves the pal forward, dampens the forces (if necessary)
	 * and updates the sprite if it exists.
	 *
	 * @param timestep  Time elapsed since last called.
	 */
	void    update(float timestep = 0.0f);

	/**
	 * Resets the pal back to its original settings
	 */
	void    reset();
};

