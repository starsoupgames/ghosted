#include "WinScene.h"

using namespace std;
using namespace cugl;

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
bool WinScene::init(const shared_ptr<AssetManager>& assets) {
    GameMode::init(assets, constants::GameMode::Win, "win");

    _quit = dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("win_quitbutton"));
    
    _ghost = dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("win_ghost"));
    
    _doe = dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("win_doe"));
    
    _seal = dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("win_seal"));
    
    _tanuki = dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("win_tanuki"));
    
    if (_quit == nullptr) {
        return false;
    }
    
    _quit->addListener([=](const string& name, bool down) {
        if (!down) {
            _mode = constants::GameMode::Start;
        }
        });
    if (_active) {
        _quit->activate();
        _ghost->activate();
        _doe->activate();
        _seal->activate();
        _tanuki->activate();
    }
    _mode = constants::GameMode::Win;
    return true;
}

/**
 * The method called to update the game mode.
 *
 * This method contains any gameplay code that is not an OpenGL call.
 *
 * @param timestep  The amount of time (in seconds) since the last frame
 */
void WinScene::update(float timestep) {
    Size dimen = Application::get()->getDisplaySize();
    dimen *= constants::SCENE_WIDTH / dimen.width;
    _quit->setVisible(_quit->isActive());
    // DEFAULT: pals win
    if (_ghostWin) {
        _doe->setVisible(false);
        _seal->setVisible(false);
        _tanuki->setVisible(false);
        _ghost->setVisible(true);
    } else {
        _ghost->setVisible(false);
        _doe->setVisible(true);
        _seal->setVisible(true);
        _tanuki->setVisible(true);
    }
    
}

/**
 * Disposes of all (non-static) resources allocated to this mode.
 */
void WinScene::dispose() {
    GameMode::dispose();
    setActive(false);
    _quit = nullptr;
    _ghost = nullptr;
    _doe = nullptr;
    _seal = nullptr;
    _tanuki = nullptr;
}

/**
 * Sets whether the scene is currently active
 *
 * @param value whether the scene is currently active
 */
void WinScene::setActive(bool value) {
    _active = value;
    if (value) {
    }
    else {
        if (_quit->isActive()) {
            _quit->deactivate();
        }
        if (_ghost->isActive()) {
            _ghost->deactivate();
        }
        if (_seal->isActive()) {
            _seal->deactivate();
        }
        if (_doe->isActive()) {
            _doe->deactivate();
        }
        if (_tanuki->isActive()) {
            _tanuki->deactivate();
        }
    }
}
