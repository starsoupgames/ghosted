#include "StartScene.h"

using namespace cugl;

/** This is adjusted by screen aspect ratio to get the height */
#define SCENE_WIDTH 1024

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
bool StartScene::init(const std::shared_ptr<AssetManager>& assets) {
    // Initialize the scene to a locked width
    Size dimen = Application::get()->getDisplaySize();
    dimen *= SCENE_WIDTH / dimen.width; // Lock the game to a reasonable resolution
    if (assets == nullptr) {
        return false;
    }
    else if (!Scene2::init(dimen)) {
        return false;
    }

    _mode = 1;

    _assets = assets;
    auto layer = assets->get<scene2::SceneNode>("textfield");
    layer->setContentSize(dimen);
    layer->doLayout(); // This rearranges the children to fit the screen
    addChild(layer);

    _field = std::dynamic_pointer_cast<scene2::TextField>(assets->get<scene2::SceneNode>("textfield_action"));
    // _field->addTypeListener([=](const std::string& name, const std::string& value) {});
    _field->addExitListener([=](const std::string& name, const std::string& value) {
        roomID = value;
        _mode = 2;
        });

    if (_active) {
        _field->activate();
    }

    // XNA nostalgia
    Application::get()->setClearColor(Color4f::CORNFLOWER);
    return true;
}

/**
 * Disposes of all (non-static) resources allocated to this mode.
 */
void StartScene::dispose() {
    _assets = nullptr;
    Scene2::dispose();
}

/**
 * Sets whether the scene is currently active
 *
 * @param value whether the scene is currently active
 */
void StartScene::setActive(bool value) {
    _active = value;
    if (value && !_field->isActive()) {
        _field->activate();
    }
    else if (!value && _field->isActive()) {
        _field->deactivate();
    }
}