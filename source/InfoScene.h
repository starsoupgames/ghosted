#pragma once
#ifndef __INFO_SCENE_H__
#define __INFO_SCENE_H__

#include <cugl/cugl.h>
#include "GameMode.h"

using namespace std;
using namespace cugl;

class InfoScene : public GameMode {
private:
    shared_ptr<scene2::Button> _back;
    shared_ptr<scene2::Button> _pal;
    shared_ptr<scene2::Button> _ghost;
public:
    InfoScene() : GameMode(constants::GameMode::Info) {}
    
    ~InfoScene() { dispose(); }
    
    bool init(const shared_ptr<AssetManager>& assets, constants::GameMode prev);

    /**
     * Disposes of all (non-static) resources allocated to this mode.
     */
    virtual void dispose() override;

    /**
     * Sets whether the scene is currently active
     *
     * @param value whether the scene is currently active
     */
    virtual void setActive(bool value) override;
};


#endif /* InfoScene_h */
