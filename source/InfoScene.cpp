#include "InfoScene.h"

using namespace std;
using namespace cugl;

bool InfoScene::init(const shared_ptr<AssetManager>& assets, constants::GameMode prev) {
    if (!GameMode::init(assets, constants::GameMode::Info, "info")) return false;
    
    _back = dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("info_xbutton"));
    if (_back == nullptr) {
        return false;
    }
    _back->addListener([=](const string& name, bool down) {
        if (!down) {
            _mode = prev;
        }
        });
    
    _pal = dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("info_palslabel"));
    if (_pal == nullptr) {
        return false;
    }
    _pal->addListener([=](const string& name, bool down) {
        if (!down) {
            assets->get<scene2::SceneNode>("info_ghostgameplay")->setVisible(false);
            assets->get<scene2::SceneNode>("info_palsgameplay")->setVisible(true);
        }
        });
    
    _ghost = dynamic_pointer_cast<scene2::Button>(assets->get<scene2::SceneNode>("info_ghostlabel"));
    if (_ghost == nullptr) {
        return false;
    }
    _ghost->addListener([=](const string& name, bool down) {
        if (!down) {
            assets->get<scene2::SceneNode>("info_ghostgameplay")->setVisible(true);
            assets->get<scene2::SceneNode>("info_palsgameplay")->setVisible(false);
        }
        });

    assets->get<scene2::SceneNode>("info_palsgameplay")->setVisible(true);
    assets->get<scene2::SceneNode>("info_ghostgameplay")->setVisible(false);
    
    if (_active) {
        _back->activate();
        _pal->activate();
        _ghost->activate();
    }
    _mode = constants::GameMode::Info;
//    _root->doLayout();
    return true;
}

void InfoScene::dispose() {
    setActive(false);
    _back->clearListeners();
    _back = nullptr;
    _pal->clearListeners();
    _pal = nullptr;
    _ghost->clearListeners();
    _ghost = nullptr;
    GameMode::dispose();
}

void InfoScene::setActive(bool value) {
    if (value) {
        if (!_back->isActive()) {
            _back->activate();
        }
    } else {
        if (_back->isActive()) {
            _back->deactivate();
        }
    }
}
