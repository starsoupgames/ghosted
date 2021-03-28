#include <cugl/cugl.h>
#include "InputController.h"

using namespace cugl;

/** Key for event handler */
#define LISTENER_KEY                1
/** This defines the joystick "deadzone" (how far we must move) */
#define JSTICK_DEADZONE  15
/** This defines the joystick radial size (for reseting the anchor) */
#define JSTICK_RADIUS    25
/** How far to display the virtual joystick above the finger */
#define JSTICK_OFFSET    80
/** Necessary length to register a swipe gesture */
#define SWIPE_LENGTH    50
/** How fast a double click must be in milliseconds */
#define DOUBLE_CLICK    400


/** The portion of the screen used for the left zone */
#define LEFT_ZONE       0.5f
/** The portion of the screen used for the right zone */
#define RIGHT_ZONE      0.5f

#pragma mark -
#pragma mark Input Controller

InputController::InputController() :
_active(false),
_movement(Vec2(0.0f,0.0f)),
_direction(Vec2(0.0f,-1.0f)),
_turnAngle(0.0f),
_pickup(false),
_keyPickUp(false),
_ljoystick(false),
_rjoystick(false)
{}

/**
 * Deactivates this input controller, releasing all listeners.
 *
 * This method will not dispose of the input controller. It can be reused
 * once it is reinitialized.
 */
void InputController::dispose() {
    if (_active) {
#ifndef CU_TOUCH_SCREEN
        Input::deactivate<Keyboard>();
#else
        Touchscreen* touch = Input::get<Touchscreen>();
        touch->removeBeginListener(LISTENER_KEY);
        touch->removeEndListener(LISTENER_KEY);
#endif
        _active = false;
    }
    
}

bool InputController::init(const Rect bounds) {
    bool success = true;
    _sbounds = bounds;
    _tbounds = Application::get()->getDisplayBounds();
    
    createZones();
    clearTouchInstance(_ltouch);
    clearTouchInstance(_rtouch);
    
#ifndef CU_TOUCH_SCREEN
    success = Input::activate<Keyboard>();
#else
    Touchscreen* touch = Input::get<Touchscreen>();
    touch->addBeginListener(LISTENER_KEY,[=](const TouchEvent& event, bool focus) {
        this->touchBeganCB(event,focus);
    });
    touch->addEndListener(LISTENER_KEY,[=](const TouchEvent& event, bool focus) {
        this->touchEndedCB(event,focus);
    });
    touch->addMotionListener(LISTENER_KEY,[=](const TouchEvent& event, const Vec2& previous, bool focus) {
        this->touchesMovedCB(event, previous, focus);
    });
    
#endif
    _active = success;
    return success;
}

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
void InputController::update(float dt) {
#ifndef CU_TOUCH_SCREEN
    // DESKTOP CONTROLS
    Keyboard* keys = Input::get<Keyboard>();

    // Map "keyboard" events to the current frame boundary
    bool _keyTurnLeft = keys->keyDown(KeyCode::ARROW_LEFT);
    bool _keyTurnRight = keys->keyDown(KeyCode::ARROW_RIGHT);
    bool _keyTurnTop = keys->keyDown(KeyCode::ARROW_UP);
    bool _keyTurnBot = keys->keyDown(KeyCode::ARROW_DOWN);
    
    bool _keyRight = keys->keyDown(KeyCode::D);
    bool _keyLeft = keys->keyDown(KeyCode::A);
    bool _keyTop = keys->keyDown(KeyCode::W);
    bool _keyBot = keys->keyDown(KeyCode::S);
    
    _movement = Vec2::ZERO;
    _direction = Vec2::ZERO;

    // MOVEMENT
    if (_keyRight) {
        _movement.x += 1.0f;
    }
    if (_keyLeft) {
        _movement.x -= 1.0f;
    }
    if (_keyTop) {
        _movement.y += 1.0f;
    }
    if (_keyBot) {
        _movement.y -= 1.0f;
    }
    
    // DIRECTION
    _direction = Vec2::ZERO;
    if (_keyTurnRight) {
        _direction.x = 1.0f;
    }
    if (_keyTurnLeft) {
        _direction.x -= 1.0f;
    }
    if (_keyTurnTop) {
        _direction.y = 1.0f;
    }
    if (_keyTurnBot) {
        _direction.y -= 1.0f;
    }
    
#endif
    
    // PICKUPS
    if (_keyPickUp) {
        _pickup = true;
    } else {
        _pickup = false;
    }
}

#pragma mark -
#pragma mark Touch Controls

/**
 * Defines the zone boundaries, so we can quickly categorize touches.
 */
void InputController::createZones() {
    _lzone = _tbounds;
    _lzone.size.width *= LEFT_ZONE;
    _rzone = _tbounds;
    _rzone.size.width *= RIGHT_ZONE;
    _rzone.origin.x = _tbounds.origin.x+_tbounds.size.width-_rzone.size.width;
}

/**
 * Returns the correct zone for the given position.
 *
 * See the comments above for a description of how zones work.
 *
 * @param  pos  a position in screen coordinates
 *
 * @return the correct zone for the given position.
 */
InputController::Zone InputController::getZone(const Vec2 pos) const {
    if (_lzone.contains(pos)) {
        return Zone::LEFT;
    } else if (_rzone.contains(pos)) {
        return Zone::RIGHT;
    }
    return Zone::UNDEFINED;
}

/**
 * Populates the initial values of the input TouchInstance
 */
void InputController::clearTouchInstance(TouchInstance& touchInstance) {
    touchInstance.touchids.clear();
    touchInstance.position = Vec2::ZERO;
}

/**
 * Returns the scene location of a touch
 *
 * Touch coordinates are inverted, with y origin in the top-left
 * corner. This method corrects for this and scales the screen
 * coordinates down on to the scene graph size.
 *
 * @return the scene location of a touch
 */
Vec2 InputController::touch2Screen(const Vec2 pos) const {
    float px = pos.x/_tbounds.size.width -_tbounds.origin.x;
    float py = pos.y/_tbounds.size.height-_tbounds.origin.y;
    Vec2 result;
    result.x = px*_sbounds.size.width +_sbounds.origin.x;
    result.y = (1-py)*_sbounds.size.height+_sbounds.origin.y;
    return result;
}

/**
 * Processes movement for the floating joystick (left side of screen)
 *
 * This will register movement as left, right, up, down (or none).  It
 * will also move the joystick anchor if the touch position moves
 * too far.
 *
 * @param  pos  the current joystick position
 */
void InputController::readLeft(const cugl::Vec2 pos) {
    Vec2 diff =  _ltouch.position-pos;

    // Reset the anchor if we drifted too far
    if (diff.lengthSquared() > JSTICK_RADIUS*JSTICK_RADIUS) {
        diff.normalize();
        diff *= (JSTICK_RADIUS+JSTICK_DEADZONE)/2;
        _ltouch.position = pos+diff;
    }
//    _ltouch.position.y = pos.y;
//    _ljoycenter = this->touch2Screen(_ltouch.position);
//    _ljoycenter.y += JSTICK_OFFSET;
    
    if (std::fabsf(diff.y) > JSTICK_DEADZONE/3 || std::fabsf(diff.x) > JSTICK_DEADZONE) {
        _ljoystick = true;
        
        // return as movement vector (x,y) normalize in move in PLAYER
        _movement = Vec2(pos.x-_ltouch.position.x, _ltouch.position.y-pos.y);
    } else {
        _ljoystick = false;
        _movement = Vec2::ZERO;
    }
}

/**
 * Processes turning input (right side of screen)
 *
 * This will register movement as right or left (or neither).
 *
 * @param  start    the start position of the candidate swipe
 * @param  stop     the end position of the candidate swipe
 * @param  current  the current timestamp of the gesture
 */
void InputController::readRight(const Vec2 start, const Vec2 stop, Timestamp current) {
    // Look for swipes up that are "long enough"
    Vec2 diff = _rtouch.position-stop;
//
//    // Reset the anchor if we drifted too far
    if (diff.lengthSquared() > JSTICK_RADIUS*JSTICK_RADIUS) {
        diff.normalize();
        diff *= (JSTICK_RADIUS+JSTICK_DEADZONE)/2;
        _rtouch.position = stop+diff;
    }
//    _rtouch.position = stop;
//    _rjoycenter = this->touch2Screen(_rtouch.position);
//    _rjoycenter.y += JSTICK_OFFSET;
    
    if (std::fabsf(diff.y) > JSTICK_DEADZONE/3 || std::fabsf(diff.x) > JSTICK_DEADZONE) {
        _rjoystick = true;
        
        // return as direction vector (x,y) normalize in setDir in PLAYER
        _direction = Vec2(stop.x-start.x, start.y-stop.y);
    } else {
        _rjoystick = false;
        _direction = Vec2::ZERO;
    }
}

#pragma mark -
#pragma mark Touch and Mouse Callbacks
/**
 * Callback for the beginning of a touch event
 *
 * @param event The associated event
 * @param focus    Whether the listener currently has focus
 */
void InputController::touchBeganCB(const TouchEvent& event, bool focus) {
    //CULog("Touch began %lld", event.touch);
    Vec2 pos = event.position;
    Zone zone = getZone(pos);
    switch (zone) {
        case Zone::LEFT:
            // Only process if no touch in zone
            if (_ltouch.touchids.empty()) {
                // Left is the floating joystick
                _ltouch.position = event.position;
                _ltouch.timestamp.mark();
                _ltouch.touchids.insert(event.touch);

                _ljoystick = true;
                _ljoycenter = touch2Screen(event.position);
                _ljoycenter.y += JSTICK_OFFSET;
            }
            break;
        case Zone::RIGHT:
            // Only process if no touch in zone
            if (_rtouch.touchids.empty()) {
                // Right is turning controls
                _keyPickUp = (event.timestamp.ellapsedMillis(_rtime) <= DOUBLE_CLICK);
                _rtouch.position = event.position;
                _rtouch.timestamp.mark();
                _rtouch.touchids.insert(event.touch);
                
                _rjoystick = true;
                _rjoycenter = touch2Screen(event.position);
                _rjoycenter.y += JSTICK_OFFSET;
            }
            break;
        default:
            CULog("Touch is out of bounds");
            break;
    }
}

 
/**
 * Callback for the end of a touch event
 *
 * @param event The associated event
 * @param focus    Whether the listener currently has focus
 */
void InputController::touchEndedCB(const TouchEvent& event, bool focus) {
    // Reset all keys that might have been set
    if (_ltouch.touchids.find(event.touch) != _ltouch.touchids.end()) {
        _ltouch.touchids.clear();
        _movement = Vec2::ZERO;
        _ljoystick = false;
    }
    else if (_rtouch.touchids.find(event.touch) !=
        _rtouch.touchids.end()) {
        _rtime = event.timestamp;
        _rtouch.touchids.clear();
        _keyPickUp = false;
        _rjoystick = false;
    }
}


/**
 * Callback for a touch moved event.
 *
 * @param event The associated event
 * @param previous The previous position of the touch
 * @param focus    Whether the listener currently has focus
 */
void InputController::touchesMovedCB(const TouchEvent& event, const Vec2& previous, bool focus) {
    Vec2 pos = event.position;
    if (_ltouch.touchids.find(event.touch) != _ltouch.touchids.end()) {
        readLeft(pos);
    }
    if (_rtouch.touchids.find(event.touch) != _rtouch.touchids.end()) {
        readRight(previous, pos, _rtouch.timestamp);
    }
}
