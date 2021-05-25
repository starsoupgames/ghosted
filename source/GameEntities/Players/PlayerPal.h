#pragma once

#include <cugl/cugl.h>
#include "../Player.h"
using namespace std;
using namespace cugl;

/** Pal Frame Sprite numbers */
#define IMG_SPOOK_CHANGE 72 // Initial spook frame
#define IMG_SPOOKED_START 94 // Idle spooked frame
#define IMG_SPOOKED_END 95 // Last idle spooked frame
#define IMG_SMOKE_FRAME_START 75 // Frame for smoke to start (refers to pal texture)

#define IMG_UNSPOOK_START 96 // First unspooking frame
#define IMG_UNSPOOK_END 116 // Last unspooking frame
#define IMG_HELP_START 117 // First help frame
#define IMG_HELP_END 135 // Last help frame

/** Pal effect sprite numbers */
#define IMG_SMOKE_START 0 // First smoke frame (refers to effect texture, NOT pal texture)
#define IMG_SMOKE_END 18 // Last smoke frame (refers to effect texture, NOT pal texture)
#define IMG_HEART_START 19 // First heart frame (refers to effect texture, NOT pal texture)
#define IMG_HEART_END 27 // Last heart frame (refers to effect texture, NOT pal texture)

/** pal helping ticks **/
#define HELPING_TICKS 100

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

    /** Spook flag for networking */
    bool _spookFlag;

    /** Unspook flag for networking */
    bool _unspookFlag;

    /** Progress while helping */
    int _helping;

    /** Progress for a pal being unspooked */
    int _unspooking;

    /** Reference to the animation node */
    shared_ptr<scene2::AnimationNode> _effectNode;

    /** Sets whether or not the pal is done being unspooked */
    bool doneUnspooking() const {
        return _unspooking == 0;
    }

    /** Returns whether or not a Pal is being unspooked */
    bool getUnspooking() const {
        return _unspooking > 0;
    }

    /** Processes the direction for the animation */
    virtual void processDirection() override;

    /** Processes the timing for a pal helping */
    virtual void processHelping();

public:
    /** Returns the amount of batteries left */
    int getBatteries() const {
        return _batteries;
    }

    /** Sets the amount of batteries */
    void setBatteries(int num) {
        _batteries = num;
    }

    /** Returns whether or not a Pal is spooked */
    bool getSpooked() const {
        return _spooked;
    }

    /** Sets whether or not a Pal is spooked */
    void setSpooked(bool spooked) {
        if (_spooked == spooked) return;
        if (!(_spookFlag || _unspookFlag)) {
            spooked ? setSpookFlag() : setUnspookFlag();
        }
        updateSpooked();
    }

    /** Updates whether or not a Pal is spooked */
    void updateSpooked() {
        if (_spookFlag) {
            _spooked = true;
        }
        if (_unspookFlag) {
            if (_spooked) {
                _unspooking = HELPING_TICKS;
            }
            _spooked = false;
        }
        _spookFlag = false;
        _unspookFlag = false;
    }

    /** Returns spook flag */
    bool getSpookFlag() const {
        return _spookFlag;
    }

    /** Sets spookFlag */
    void setSpookFlag() {
        if (_spooked || getUnspooking()) return;
        _spookFlag = true;
    }

    /** Returns unspook flag */
    bool getUnspookFlag() const {
        return _unspookFlag;
    }

    /** Sets spookFlag */
    void setUnspookFlag() {
        if (!_spooked) return;
        _unspookFlag = true;
    }


    /** Returns whether or not a Pal is helping */
    bool getHelping() const {
        return _helping > 0;
    }

    /** Sets the pal as helping */
    void setHelping() {
        if (!getHelping()) {
            _helping = HELPING_TICKS;
        }
    }

    /** Sets whether or not the trap is done triggering */
    bool doneHelping() const {
        return _helping == 0;
    }

    /** Creates a Pal with the default values */
    Pal() : Player(7, 1), _batteries(0), _spooked(false), _spookFlag(false), _unspookFlag(false), _helping(0), _unspooking(0) {};

    /** Releases all resources allocated with this Pal */
    ~Pal() { dispose(); }

    virtual void dispose() {
        Player::dispose();
        _effectNode = nullptr;
    }

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

    /** Increments/decrements the battery counter */
    void addBattery(int num) {
        _batteries += num;
    }

    /** Get player type */
    virtual constants::PlayerType getType() override {
        return constants::PlayerType::Pal;
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
