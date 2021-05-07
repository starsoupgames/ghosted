#pragma once
/**

This PlayerGhost class contains information about the Ghost's texture, the amount of traps it has left,
and methods for the Ghost's actions

*/

#include <cugl/cugl.h>
#include "../Player.h"
using namespace std;
using namespace cugl;

/** Pal Frame Sprite numbers */
#define IMG_SPOOKING_START 72 // Initial spooking frame
#define IMG_SPOOKING_END 95 // Last spooking frame


class Ghost : public Player {
private:

    /** Amount of Traps left */
    int _traps;

    /** Whether the Ghost has been revealed recently */
    bool _tagged;

    /** Amount of frames before the Ghost is untagged */
    int _tagTimer;

    /** Amount of frames before the Ghost is done spooking */
    int _spookingTimer;

    /** Whether a Ghost is spooking or not */
    bool _spooking;

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

    /** Returns whether or not the ghost is spooking */
    bool getSpooking() const {
        return _spooking;
    }

    /** Sets whether or not a Ghost is spooking */
    void setSpooking(bool spooking) {
        _spooking = spooking;
    }

    /** Creates a Ghost with the default values */
    Ghost() : Player(6), _tagTimer(0) {};

    /** Releaes all resources allocated with this Ghost */
    ~Ghost() { }

    /**
    * @return a newly allocated Ghost at the origin.
    */
    static shared_ptr<Ghost> alloc() {
        shared_ptr<Ghost> result = make_shared<Ghost>();
        return (result->init() ? result : nullptr);
    }

    static shared_ptr<Ghost> alloc(const Vec2& pos) {
        shared_ptr<Ghost> result = make_shared<Ghost>();
        return (result->init(pos) ? result : nullptr);
    };

    /** Get player type */
    virtual constants::PlayerType getType() override {
        return constants::PlayerType::Ghost;
    }

    /**
     * @param value The Player node.
     * @param shadow The Shadow node.
     */
    virtual void setNode(const shared_ptr<scene2::AnimationNode>& value, const std::shared_ptr<scene2::PolygonNode>& shadow) override;

    /** Processes the direction for the animation */
    virtual void processDirection() override;

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
