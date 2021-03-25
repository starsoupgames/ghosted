#pragma once
#ifndef __GHOSTED_START_SCENE_H__
#define __GHOSTED_START_SCENE_H__

#include <cugl/cugl.h>

class StartScene : public cugl::Scene2 {
protected:
    /** The asset manager for this game mode. */
    std::shared_ptr<cugl::AssetManager> _assets;

    std::shared_ptr<cugl::scene2::TextField> _field;

    string _roomID;

    bool _startGame;

    unsigned _mode;

public:
#pragma mark -
#pragma mark Constructors
    /**
     * Creates a new scene with the default values.
     *
     * This constructor does not allocate any objects or start the controller.
     * This allows us to use a controller without a heap pointer.
     */
    StartScene() : Scene2(), _startGame(false), _mode(1), _roomID("") {}

    /**
     * Disposes of all (non-static) resources allocated to this mode.
     *
     * This method is different from dispose() in that it ALSO shuts off any
     * static resources, like the input controller.
     */
    ~StartScene() { dispose(); }

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
    bool init(const std::shared_ptr<cugl::AssetManager>& assets);

    static std::shared_ptr<StartScene> alloc(const std::shared_ptr<cugl::AssetManager>& assets) {
        std::shared_ptr<StartScene> result = std::make_shared<StartScene>();
        return (result->init(assets) ? result : nullptr);
    }

    /**
     * Sets whether the scene is currently active
     *
     * @param value whether the scene is currently active
     */
    virtual void setActive(bool value) override;

    unsigned getMode() {
        return _mode;
    };

    string getRoomID() {
        return _roomID;
    };
};

#endif /* __UI_TEXT_SCENE_H__ */
