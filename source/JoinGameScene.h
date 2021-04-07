#pragma once
#ifndef __JOIN_GAME_SCENE_H__
#define __JOIN_GAME_SCENE_H__

#include <cugl/cugl.h>
#include "GameMode.h"

using namespace std;
using namespace cugl;

class JoinGameScene : public GameMode {
private:
    /** Text field */
    shared_ptr<scene2::TextField> _field;

    /** Room ID to connect to */
    string _roomID;

public:
    string getRoomID() {
        return _roomID;
    }

public:
    /**
     * Creates a new scene with the default values.
     *
     * This constructor does not allocate any objects or start the controller.
     * This allows us to use a controller without a heap pointer.
     */
    JoinGameScene() : GameMode(constants::GameMode::JoinGame) {}

    /**
     * Disposes of all (non-static) resources allocated to this mode.
     *
     * This method is different from dispose() in that it ALSO shuts off any
     * static resources, like the input controller.
     */
    ~JoinGameScene() { dispose(); }

    /**
     * Initializes the controller contents, and starts the game
     *
     * The constructor does not allocate any objects or memory.  This allows
     * us to have a non-pointer reference to this controller, reducing our
     * memory allocation.  Instead, allocation happens in this method.
     *
     * @param assets    The (loaded) assets for this game mode
     *
     * @return true if the controller is initialized properly, false otherwise.
     */
    bool init(const shared_ptr<AssetManager>& assets);

    /**
     * Sets whether the scene is currently active
     *
     * @param value whether the scene is currently active
     */
    virtual void setActive(bool value) override;
};

#endif /* __JOIN_GAME_SCENE_H__ */
