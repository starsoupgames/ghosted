#pragma once
/**

This PlayerPal class contains information about the Pal's texture, the amount of batteries it has,
and methods for its actions

*/

#include <cugl/cugl.h>
#include "GameEntity.h"
using namespace std;
using namespace cugl;

/** Pal Frame Sprite numbers */
//TODO: THESE VALUES MUST BE ADJUSTED TO PAL'S AND ALSO HAVE FRONT, BACK, LEFT, RIGHT FRAMES
#define PAL_IMG_RIGHT   0   // Right idle frame
#define PAL_IMG_LEFT 21   // Left idle frame
#define PAL_IMG_FRONT 42      // Front idle frame
#define PAL_IMG_BACK 63      // Back idle frame
#define PAL_IMG_LAST 83

//TODO: THESE VALUES MUST BE ADJUSTED TO PAL'S AND ALSO HAVE FRONT, BACK, LEFT, RIGHT FRAMES
/** Number of rows and cols in film strip */
//number of cols in sheet
#define PAL_IMG_ROWS   4
//number of rows in sheet
#define PAL_IMG_COLS   21
//number of sprites in sheet
#define PAL_IMG_SIZE  84

class Pal : public GameEntity {
private:

	/** Which Pal this is */
	int _id;

	/** Amount of Traps left */
	int _batteries;

	/** Whether a Pal is spooked or not */
	bool _spooked;

	/** Reference to the Pal's texture */
  
	//shared_ptr<Texture> _palTexture;

	/** Reference to the entity's sprite for drawing */
	shared_ptr<scene2::AnimationNode> _palTexture;

	/** Whether we are mid animation */
	//bool _occupied;
	
	/** Whether we are idle */
	bool _idle;

	/** Direction currently in */
	bool _left;
	bool _right;
	bool _front;
	bool _back;


private:
	void advanceFrame();
	void determineAction();


protected:
	/** Current forward movement */
	float _forward;

	/** Current sideways movement  */
	float _side;

	float _turn;

	/** Current pal movement */
	cugl::Vec2  _move;

public:

	/** Returns the id of the Pal */
	int getId() const {
		return _id;
	}

	/** Returns the amount of batteries left */
	int getBatteries() const {
		return _batteries;
	}

	/** Returns whether or not a Pal is spooed */
	int getSpooked() const {
		return _spooked;
	}

	/** Returns the reference to the battery's texture */
	const shared_ptr <scene2::AnimationNode> getTexture() const {
		return _palTexture;
	}

	/** Sets the amount of traps left */
	void setTraps(int num) {
		_batteries = num;
	}

	/** Sets whether or not a Pal is spooked */
	void setSpooked(bool spooked) {
		_spooked = spooked;
	}

	/**
	 * Returns the current forward force on the pal
	 *
	 * @return The current forward force on the pal
	 */
	float getForward() { return _forward; }

	/**
	 * Sets the current forward force on the pal
	 *
	 * @param value The pal forward force
	 */
	void setForward(float value) { _forward = value; }

	/**
	 * Returns the current forward force on the pal
	 *
	 * @return The current forward force on the pal
	 */
	float getSide() { return _side; }

	/**
	 * Sets the current forward force on the pal
	 *
	 * @param value The pal forward force
	 */
	void setSide(float value) { _side = value; }


	/** Creates a Pal with the default values */
	Pal(void) : GameEntity() {};

	/** Releases all resources allocated with this Pal */
	~Pal(void) { dispose(); }

	/** Releases all resources allocated with this Pal */
	void dispose();

	/** Initializes a new Pal at the given location with the given id */
	bool init(const cugl::Vec2& pos);

	bool init() { return init(cugl::Vec2::ZERO); }

	/**
	* Returns a newly allocated Pal at the origin
	* @return a newly allocated Pal at the origin.
	*/
	static std::shared_ptr<Pal> alloc() {
		std::shared_ptr<Pal> result = std::make_shared<Pal>();
		return (result->init() ? result : nullptr);
	}

	static std::shared_ptr<Pal> alloc(const cugl::Vec2& pos) {
		std::shared_ptr<Pal> result = std::make_shared<Pal>();
		return (result->init(pos) ? result : nullptr);
	};

	/** Processes the animation and vision cone to process a turn */
	void processTurn(float turn) { _turn = turn; };

	/** Moves the Pal in the corresponding direction */
	void move(Vec2 direction);

	/** Places a battery in the nearby slot */
	void placeBattery();

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
	void update(float timestep = 0.0f);

	/**
	 * Resets the pal back to its original settings
	 */
	void reset();

};