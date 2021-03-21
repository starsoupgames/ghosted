#pragma once
/**

This class is the superclass which all other GameEntities subclass. It contains their texture, and location, and a node
for animations

*/

#include <cugl/cugl.h>
using namespace std;
using namespace cugl;

class GameEntity {

private:
	/** Location of the entity */
	Vec2 _loc;

	/** Reference to the entity's sprite for drawing */
	shared_ptr<scene2::AnimationNode> _animationNode;

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
	const shared_ptr<scene2::AnimationNode> getNode() const {
		return _animationNode;
	}

	/**
	Sets the position of the entity
	*/
	void setLoc(Vec2 position) {
		_loc = position;
	}

	/**
	Sets the entity's animation node
	*/
	void setNode(const shared_ptr<scene2::AnimationNode> node) {
		_animationNode = node;
	}

	/** Creates a GameEntity with the default values */
	GameEntity();

	/** Initializes a new GameEntity at given location */
	bool init(scene2::AnimationNode node, Vec2 loc);

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
};