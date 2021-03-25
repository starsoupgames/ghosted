#pragma once

#include <cugl/cugl.h>
#include "../Player.h"
using namespace std;
using namespace cugl;

/** Pal Frame Sprite numbers */
#define PAL_IMG_RIGHT   0   // Right idle frame
#define PAL_IMG_LEFT 21   // Left idle frame
#define PAL_IMG_FRONT 42      // Front idle frame
#define PAL_IMG_BACK 63      // Back idle frame
#define PAL_IMG_LAST 83

/**
This PlayerPal class contains information about the Pal's texture, the amount of batteries it has,
and methods for its actions
*/
class Pal : public Player {
private:
	/** Amount of batteries left */
	int _batteries;

	/** Whether a Pal is spooked or not */
	bool _spooked;

	/** Speed of the Pal */
	float speed;
	
	/** Whether we are idle */
	bool _idle;

	/** Direction currently in */
	bool _left;
	bool _right;
	bool _front;
	bool _back;

	void advanceFrame();

	void determineAction();

	float _turn;

public:
	/** Returns the amount of batteries left */
	int getBatteries() const {
		return _batteries;
	}

	/** Returns whether or not a Pal is spooed */
	bool getSpooked() const {
		return _spooked;
	}

	/** Sets the amount of batteries */
	void setBatteries(int num) {
		_batteries = num;
	}

	/** Sets whether or not a Pal is spooked */
	void setSpooked(bool spooked) {
		_spooked = spooked;
	}

	/** Creates a Pal with the default values */
	Pal() : Player(), speed(5) {};

	/** Releases all resources allocated with this Pal */
	~Pal() { }

	/**
	* @return a newly allocated Pal at the origin.
	*/
	static shared_ptr<Pal> alloc() {
		shared_ptr<Pal> result = make_shared<Pal>();
		return (dynamic_pointer_cast<Player>(result)->init() ? result : nullptr);
	}

	static shared_ptr<Pal> alloc(const Vec2& pos) {
		shared_ptr<Pal> result = make_shared<Pal>();
		return (dynamic_pointer_cast<Player>(result)->init(pos) ? result : nullptr);
	};

	/** Processes the animation and vision cone to process a turn */
	void processTurn(float turn) { 
		_turn = turn; 
	};

	/** Places a battery in the nearby slot */
	void placeBattery();

	/** Increments/decrements the battery counter */
	void addBattery(int num) {
		_batteries += num;
	}

	/** Get player type */
	virtual uint8_t getType() override {
		return Type::Pal;
	}

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
	virtual void update(float timestep = 0.0f) override;

	/**
	 * Resets the pal back to its original settings
	 */
	void reset();

};