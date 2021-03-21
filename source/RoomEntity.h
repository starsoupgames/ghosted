#pragma once
/**

This class is the superclass which all other RoomEntities subclass. It contains their texture, and position, and a node
for animations

*/

#include <cugl/cugl.h>
using namespace std;
using namespace cugl;

class RoomEntity {

private:
	/** Position of the entity */
	Vec2 _pos;

	/** Reference to the entity's sprite for drawing */
	shared_ptr<scene2::AnimationNode> _animationNode;

public:

	/**
	Returns the position of the entity
	*/
	Vec2 getPos() const {
		return _pos;
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
	void setPos(Vec2 position) {
		_pos = position;
	}

	/**
	Sets the entity's animation node
	*/
	void setNode(const shared_ptr<scene2::AnimationNode> node) {
		_animationNode = node;
	}

	/** Creates a RoomEntity with the default values */
	RoomEntity();

	/** Initializes a new RoomEntity at given position */
	bool init(scene2::AnimationNode node, Vec2 pos);

	/** Releases all resources allocated with this entity */
	void dispose();
};