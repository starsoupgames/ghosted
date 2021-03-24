#pragma once

#include <cugl/cugl.h>
#include "GameEntity.h"
using namespace std;
using namespace cugl;

/**
This class contains information about the Player.
*/
class Player : public GameEntity {
protected:
	/** Reference to the animation node */
	shared_ptr<scene2::AnimationNode> _node;

	/** Whether we are idle */
	bool _idle;

	/** Direction currently in */
	bool _left;
	bool _right;
	bool _front;
	bool _back;

	float _turn;

	/** Current Player movement */
	Vec2 _move;

public:

	/** Returns the reference to the animation node */
	const shared_ptr <scene2::AnimationNode> getNode() const {
		return _node;
	}

	/** Returns the current direction of the Player
	 *
	 * @return the current direction of the Player
	 */
	string getDirection() {
		if (_left) {
			return "left";
		}
		else if (_right) {
			return "right";
		}
		else if (_front) {
			return "front";
		}
		else {
			return "back";
		}
	}

	/** Creates a Player with the default values */
	Player() : GameEntity() {};

	/** Releases all resources allocated with this Player */
	virtual ~Player() { dispose(); }

	/** Releases all resources allocated with this Player */
	void dispose();

	/** Initializes a new Player at the given location */
	bool init(const Vec2& pos);

	bool init() { return init(Vec2::ZERO); }

	/** Moves the Player */
	void move(Vec2 move) {
		_move = move;
	}

	/**
	 * @param value The Player node.
	 */
	void setNode(const shared_ptr<scene2::AnimationNode>& value);

	/**
	 * Updates the state of the model
	 *
	 * This method moves the Player forward, dampens the forces (if necessary)
	 * and updates the sprite if it exists.
	 *
	 * @param timestep  Time elapsed since last called.
	 */
	void update(float timestep = 0.0f);

	/**
	 * Resets the Player back to its original settings
	 */
	void reset();
};
