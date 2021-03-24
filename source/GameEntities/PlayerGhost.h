#pragma once
/**

This PlayerGhost class contains information about the Ghost's texture, the amount of traps it has left,
and methods for the Ghost's actions

*/

#include <cugl/cugl.h>
#include "GameEntity.h"
using namespace std;
using namespace cugl;

/** Pal Frame Sprite numbers */
//TODO: THESE VALUES MUST BE ADJUSTED TO PAL'S AND ALSO HAVE FRONT, BACK, LEFT, RIGHT FRAMES
#define GHOST_IMG_RIGHT   0   // Right idle frame
#define GHOST_IMG_LEFT 21   // Left idle frame
#define GHOST_IMG_FRONT 42      // Front idle frame
#define GHOST_IMG_BACK 63      // Back idle frame
#define GHOST_IMG_LAST 83

class Ghost : public GameEntity {
private:

	/** Amount of Traps left */
	int _traps;

	/** Speed of the Pal */
	float speed;

	/** Reference to the animation node */
	shared_ptr<scene2::AnimationNode> _ghostNode;

	/** Whether we are idle */
	bool _idle;

	/** Whether the Ghost has been revealed recently */
	bool _tagged;

	/** Direction currently in */
	bool _left;
	bool _right;
	bool _front;
	bool _back;

private:
	void advanceFrame();
	void determineAction();


protected:
	/** Current ghost movement */
	cugl::Vec2 _move;

public:

	/** @return the amount of traps left */
	int getTraps() const {
		return _traps;
	}

	/** @return whether the Ghost has been revealed recently */
	bool getTagged() const {
		return _tagged;
	}

	const shared_ptr <scene2::AnimationNode> getNode() const {
		return _ghostNode;
	}

	/** Sets the amount of traps left */
	void setTraps(int num) {
		_traps = num;
	}

	/**
	 * Sets whether or not the Ghost has been revealed recently
	 *
	 * @param value Whether or not the Ghost has been revealed recently
	 */
	void setTagged(float value) { _tagged = value; }

	/** Creates a Ghost with the default values */
	Ghost() : GameEntity(), speed(5) {};

	/** Releaes all resources allocated with this Ghost */
	~Ghost() { dispose(); }

	/** Releases all resources allocated with this Ghost */
	void dispose();

	/** Initializes a new Ghost at the given location */
	//bool init(int x, int y);

	/** Initializes a new Ghost at the given location with the given id */
	bool init(const cugl::Vec2& pos);

	bool init() { return init(cugl::Vec2::ZERO); }

	/**
	* Returns a newly allocated Pal at the origin
	* @return a newly allocated Pal at the origin.
	*/
	static std::shared_ptr<Ghost> alloc() {
		std::shared_ptr<Ghost> result = std::make_shared<Ghost>();
		return (result->init() ? result : nullptr);
	}

	static std::shared_ptr<Ghost> alloc(const cugl::Vec2& pos) {
		std::shared_ptr<Ghost> result = std::make_shared<Ghost>();
		return (result->init(pos) ? result : nullptr);
	};


	/** Moves the Ghost in the corresponding direction */
	void move(Vec2 move) {
		_move = move;
	}

	/**
	 * Sets the film strip representing this ghosts.
	 *
	 * Setting this to nullptr clears the value.
	 *
	 * @param value The ghost film strip.
	 */
	void setNode(const std::shared_ptr<cugl::scene2::AnimationNode>& value);

	/**
	 * Updates the state of the model
	 *
	 * This method moves the ghost forward, dampens the forces (if necessary)
	 * and updates the sprite if it exists.
	 *
	 * @param timestep  Time elapsed since last called.
	 */
	void update(float timestep = 0.0f);

	/**
	 * Resets the ghost back to its original settings
	 */
	void reset();

	/** Triggers the nearest trap */
	void trigger();
};