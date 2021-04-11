#pragma once
#ifndef __GAME_MODE_H__
#define __GAME_MODE_H__

#include <cugl/cugl.h>
#include "Constants.h"

using namespace cugl;

class GameMode : public Scene2 {
protected:
    /** The asset manager for this game mode. */
    shared_ptr<AssetManager> _assets;

    /** The game mode enum value. */
    constants::GameMode _mode;

    /** The game mode's root node. */
    shared_ptr<scene2::SceneNode> _root;

public:
    /**
     * Creates a new scene with the default values.
     *
     * This constructor does not allocate any objects or start the controller.
     * This allows us to use a controller without a heap pointer.
     */
    GameMode() : GameMode(constants::GameMode::None) {}

    GameMode(constants::GameMode mode) : Scene2(), _mode(mode) {}

    /**
     * Disposes of all (non-static) resources allocated to this mode.
     *
     * This method is different from dispose() in that it ALSO shuts off any
     * static resources, like the input controller.
     */
    ~GameMode() { dispose(); }

    /**
     * Disposes of all (non-static) resources allocated to this mode.
     */
    virtual void dispose() override;

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

    bool init(const shared_ptr<AssetManager>& assets, constants::GameMode mode);

    bool init(const shared_ptr<AssetManager>& assets, constants::GameMode mode, const string node);

    constants::GameMode getMode() {
        return _mode;
    };
};

#endif /* __GAME_MODE_H__ */
