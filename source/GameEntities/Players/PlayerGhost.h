#pragma once
/**

This PlayerGhost class contains information about the Ghost's texture, the amount of traps it has left,
and methods for the Ghost's actions

*/

#include <cugl/cugl.h>
#include "../Player.h"
using namespace std;
using namespace cugl;

/** Ghost Frame Sprite numbers */
//TODO: THESE VALUES MUST BE ADJUSTED TO GHOST'S AND ALSO HAVE FRONT, BACK, LEFT, RIGHT FRAMES
#define GHOST_IMG_RIGHT   0   // Right idle frame
#define GHOST_IMG_LEFT 21   // Left idle frame
#define GHOST_IMG_FRONT 42      // Front idle frame
#define GHOST_IMG_BACK 63      // Back idle frame
#define GHOST_IMG_LAST 83

class Ghost : public Player {
private:

	/** Amount of Traps left */
	int _traps;

	/** Speed of the Pal */
	float speed;

	/** Whether we are idle */
	bool _idle;

	/** Whether the Ghost has been revealed recently */
	bool _tagged;

	/** Amount of frames before the Ghost is untagged */
	int _tagTimer;

	/** Direction currently in */
	bool _left;
	bool _right;
	bool _front;
	bool _back;

	void advanceFrame();

	void determineAction();

public:

	/** @return the amount of traps left */
	int getTraps() const {
		return _traps;
	}

	/** @return whether the Ghost has been revealed recently */
	bool getTagged() const {
		return _tagged;
	}

	/** @return the amount of frames before the Ghost is untagged */
	int getTimer() const {
		return _tagTimer;
	}

	/** Sets the amount of frames before the Ghost is untagged */
	void setTimer(int value) { _tagTimer = value; }

	/** Sets the amount of traps left */
	void setTraps(int num) {
		_traps = num;
	}

	/**
	 * Sets whether or not the Ghost has been revealed recently
	 *
	 * @param value Whether or not the Ghost has been revealed recently
	 */
	void setTagged(bool value) { _tagged = value; }

	/** Creates a Ghost with the default values */
	Ghost() : Player(), speed(5) {};

	/** Releaes all resources allocated with this Ghost */
	~Ghost() { }

	/**
	* @return a newly allocated Ghost at the origin.
	*/
	static shared_ptr<Ghost> alloc() {
		shared_ptr<Ghost> result = make_shared<Ghost>();
		return (dynamic_pointer_cast<Player>(result)->init() ? result : nullptr);
	}

	static shared_ptr<Ghost> alloc(const Vec2& pos) {
		shared_ptr<Ghost> result = make_shared<Ghost>();
		return (dynamic_pointer_cast<Player>(result)->init(pos) ? result : nullptr);
	};

	/** Triggers the nearest trap */
	void trigger();

	/** Get player type */
	virtual uint8_t getType() override {
		return Type::Ghost;
	}

	/**
	 * Sets the film strip representing this ghosts.
	 *
	 * Setting this to nullptr clears the value.
	 *
	 * @param value The ghost film strip.
	 */
	void setNode(const shared_ptr<scene2::AnimationNode>& value);

	/**
	 * Updates the state of the model
	 *
	 * This method moves the ghost forward, dampens the forces (if necessary)
	 * and updates the sprite if it exists.
	 *
	 * @param timestep  Time elapsed since last called.
	 */
	virtual void update(float timestep = 0.0f) override;

	/**
	 * Resets the ghost back to its original settings
	 */
	void reset();
};
