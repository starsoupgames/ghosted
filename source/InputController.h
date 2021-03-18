#ifndef __INPUT_CONTROLLER_H__
#define __INPUT_CONTROLLER_H__
#include <cugl/cugl.h>

class InputController {
private:
    /** Whether or not this input is active */
    bool _active;
    /** Player id, to identify which keys map to this player */
    int _player;
    
    /** Player movement vector */
    cugl::Vec2 _movement;
    
    /** Player turn input */
    float _turning;
    
    /** Whether the left arrow key is down */
    bool  _keyLeft;
    /** Whether the right arrow key is down */
    bool  _keyRight;
    /** Whether the up arrow key is down */
    bool  _keyTop;
    /** Whether the down arrow key is down */
    bool  _keyBot;
    /** Whether the A (left turn) key is down */
    bool _keyTurnLeft;
    /** Whether the D (right turn) key is down */
    bool _keyTurnRight;
    
protected:
    // The screen is divided into four zones: Left, Bottom, Right and Main/
    // These are all shown in the diagram below.
    //
    //   |---------------|
    //   |       |       |
    //   | L     |  R    |
    //   |       |       |
    //   -----------------
    //
    // The meaning of any touch depends on the zone it begins in.
    
    /** Information representing a single "touch" (possibly multi-finger) */
    struct TouchInstance {
        /** The anchor touch position (on start) */
        cugl::Vec2 position;
        /** The current touch time */
        cugl::Timestamp timestamp;
        /** The touch id(s) for future reference */
        std::unordered_set<Uint64> touchids;
    };
    
    /** Enumeration identifying a zone for the current touch */
    enum class Zone {
        /** The touch was not inside the screen bounds */
        UNDEFINED,
        /** The touch was in the left zone (as shown above) */
        LEFT,
        /** The touch was in the right zone (as shown above) */
        RIGHT,
    };
    
    /** The bounds of the entire game screen (in touch coordinates) */
    cugl::Rect _tbounds;
    /** The bounds of the entire game screen (in scene coordinates) */
    cugl::Rect _sbounds;
    /** The bounds of the left touch zone (movement) */
    cugl::Rect _lzone;
    /** The bounds of the right touch zone (turning) */
    cugl::Rect _rzone;
    
    /** The current touch location on the left side */
    TouchInstance _ltouch;
    /** The current touch location on the right side */
    TouchInstance _rtouch;
    
    /** Whether the virtual joystick is active */
    bool _joystick;
    /** The position of the virtual joystick */
    cugl::Vec2 _joycenter;
    
    /**
     * Defines the zone boundaries, so we can quickly categorize touches.
     */
    void createZones();
    
    /**
     * Returns the correct zone for the given position.
     *
     * See the comments above for a description of how zones work.
     *
     * @param  pos  a position in screen coordinates
     *
     * @return the correct zone for the given position.
     */
    Zone getZone(const cugl::Vec2 pos) const;
    
    /**
     * Populates the initial values of the TouchInstances
     */
    void clearTouchInstance(TouchInstance& touchInstance);
    
    /**
     * Returns the scene location of a touch
     *
     * Touch coordinates are inverted, with y origin in the top-left
     * corner. This method corrects for this and scales the screen
     * coordinates down on to the scene graph size.
     *
     * @return the scene location of a touch
     */
    cugl::Vec2 touch2Screen(const cugl::Vec2 pos) const;

public:
    /**
     * Returns the player for this controller
     *
     * @return the player for this controller
     */
    int getPlayer() const {
        return _player;
    }
    
    /**
     * Returns the movement in X and Y directions
     *
     * @return the movement vector for this controller
     */
    cugl::Vec2 getMove() const {
        return _movement;
    }
    
    /**
     * Returns the turning direction
     *
     * @return the turning value for this controller (-1, 0, or 1)
     */
    int getTurn() const {
        return _turning;
    }
    
    /**
     * Creates a new input controller with the default settings
     *
     * To use this controller, you will need to initialize it first
     */
    InputController();

    /**
     * Disposses this input controller, releasing all resources.
     */
    ~InputController() { dispose(); }
    
    /**
     * Deactivates this input controller, releasing all listeners.
     *
     * This method will not dispose of the input controller. It can be reused
     * once it is reinitialized.
     */
    void dispose();
    
    /**
     * Initializes a new input controller for the specified player.
     *
     * @param id Player id number (0..1)
     * @param bounds screen bounds
     *
     * @return true if the player was initialized correctly
     */
    bool init(int id, const cugl::Rect bounds); 

#pragma mark -
#pragma mark Input Detection
    /**
     * Returns true if the input handler is currently active
     *
     * @return true if the input handler is currently active
     */
    bool isActive( ) const { return _active; }

    /**
     * Processes the currently cached inputs.
     *
     * This method is used to to poll the current input state.  This will poll the
     * keyboad and accelerometer.
     *
     * This method also gathers the delta difference in the touches. Depending on
     * the OS, we may see multiple updates of the same touch in a single animation
     * frame, so we need to accumulate all of the data together.
     */
    void update(float dt);

    /**
     * Clears any buffered inputs so that we may start fresh.
     */
    void clear();
    
    /**
     * Reads the movement input for a gesture on the left side of the screen.
     *
     *@param pos the current position of the joystick
     *
     */
    void readMove(const cugl::Vec2 pos);
    
    /**
     * Reads a turn input for this player on the right side of the screen.
     *
     * @param  start    the start position of the candidate swipe
     * @param  stop     the end position of the candidate swipe
     * @param  current  the current timestamp of the gesture
     */
    void readTurn(const cugl::Vec2 start, const cugl::Vec2 stop, cugl::Timestamp current);
    
#pragma mark -
#pragma mark Touch and Mouse Callbacks
    /**
     * Callback for the beginning of a touch event
     *
     * @param event The associated event
     * @param focus    Whether the listener currently has focus
     *
     */
    void touchBeganCB(const cugl::TouchEvent& event, bool focus);

    /**
     * Callback for the end of a touch event
     *
     * @param event The associated event
     * @param focus    Whether the listener currently has focus
     */
    void touchEndedCB(const cugl::TouchEvent& event, bool focus);
  
    /**
     * Callback for a mouse release event.
     *
     * @param event The associated event
     * @param previous The previous position of the touch
     * @param focus    Whether the listener currently has focus
     */
    void touchesMovedCB(const cugl::TouchEvent& event, const cugl::Vec2& previous, bool focus);
};

#endif /* __INPUT_CONTROLLER_H__ */
