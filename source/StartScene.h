#pragma once
#ifndef __START_SCENE_H__
#define __START_SCENE_H__

#include <cugl/cugl.h>
#include "GameMode.h"

using namespace std;
using namespace cugl;

class StartScene : public GameMode {
private:
    shared_ptr<scene2::Button> _create;
    shared_ptr<scene2::Button> _join;
    shared_ptr<scene2::Button> _muteButton;
    shared_ptr<scene2::Button> _unmuteButton;
    shared_ptr<scene2::Button> _info;

public:
    /**
     * Creates a new scene with the default values.
     *
     * This constructor does not allocate any objects or start the controller.
     * This allows us to use a controller without a heap pointer.
     */
    StartScene() : GameMode(constants::GameMode::Start) {}

    /**
     * Disposes of all (non-static) resources allocated to this mode.
     *
     * This method is different from dispose() in that it ALSO shuts off any
     * static resources, like the input controller.
     */
    ~StartScene() { dispose(); }

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
     * Disposes of all (non-static) resources allocated to this mode.
     */
    virtual void dispose() override;
    
    /**
     * @param timestep  The amount of time (in seconds) since the last frame
     */
    void update(float timestep) override;

    /**
     * Sets whether the scene is currently active
     *
     * @param value whether the scene is currently active
     */
    virtual void setActive(bool value) override;
};

#endif /* __START_SCENE_H__ */
