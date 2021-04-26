#pragma once

#include <cugl/cugl.h>
#include "../Player.h"
using namespace std;
using namespace cugl;

/** Pal Frame Sprite numbers */
#define IMG_LEFT 0   // Left idle frame
#define IMG_RIGHT 23   // Left idle frame
#define IMG_FRONT 24  // Front idle frame
#define IMG_BACK 48   // Back idle frame
#define IMG_LAST 71 // Last walk back frame
#define IMG_SPOOK_CHANGE 72 // Initial spook frame
#define IMG_SPOOKED_START 94 // Idle spooked frame
#define IMG_SPOOKED_END 95 // Last idle spooked frame
#define IMG_SMOKE_FRAME_START 75 // Frame for smoke to start (refers to pal texture)
#define IMG_SMOKE_END 18 // Last smoke frame (refers to smoke texture, NOT pal texture)


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

    /** Reference to the animation node */
    shared_ptr<scene2::AnimationNode> _smokeNode;

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
    Pal() : Player(5), _batteries(0), _spooked(false) {};

    /** Releases all resources allocated with this Pal */
    ~Pal() { }

    /**
    * @return a newly allocated Pal at the origin.
    */
    static shared_ptr<Pal> alloc() {
        shared_ptr<Pal> result = make_shared<Pal>();
        return (result->init() ? result : nullptr);
    }

    static shared_ptr<Pal> alloc(const Vec2& pos) {
        shared_ptr<Pal> result = make_shared<Pal>();
        return (result->init(pos) ? result : nullptr);
    };

    /** Processes the direction for the animation */
    virtual void processDirection() override;

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
     * @param value The Player node.
     * @param shadow The Shadow node.
     */
    virtual void setNode(const std::shared_ptr<scene2::AnimationNode>& node, const std::shared_ptr<scene2::PolygonNode>& shadow, const std::shared_ptr<scene2::AnimationNode>& smoke);

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
