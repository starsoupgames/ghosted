#include "GameScene.h"

using namespace cugl;
using namespace std;

/** This is adjusted by screen aspect ratio to get the height */
#define SCENE_WIDTH 1024

#define CONE_WIDTH 75
#define CONE_LENGTH 200

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

bool GameScene::init(const std::shared_ptr<cugl::AssetManager>& assets) {
    // Initialize the scene to a locked width
    Size dimen = Application::get()->getDisplaySize();
    Rect bounds = Application::get()->getSafeBounds();
    Application::get()->setClearColor(Color4f::BLACK);
    dimen *= SCENE_WIDTH / dimen.width;
    if (assets == nullptr) {
        return false;
    }
    if (!Scene2::init(dimen)) {
        return false;
    }

    _mode = 2;

    // Init data models
    _networkData = NetworkData::alloc();
    CUAssertLog(_networkData != nullptr, "NetworkData failed allocation.");

    // Init controllers
    _input.init(bounds);
    _network.init(_networkData);
    _collision.init();

    _assets = assets;

    _root = scene2::OrderedNode::allocWithOrder(scene2::OrderedNode::Order::ASCEND);
    _root->addChild(_assets->get<scene2::SceneNode>("game"));
    _root->setContentSize(dimen);
    _root->doLayout();
    addChild(_root);

    // Initialize the countdown
    _countdown = 0;

    // Initialize ending messages
    _winNode = scene2::Label::alloc("You win!", _assets->get<Font>("felt32"));
    _winNode->setAnchor(Vec2::ANCHOR_CENTER);
    _winNode->setPosition(dimen.width / 2.0f, dimen.height / 2.0f);
    _winNode->setForeground(Color4::YELLOW);

    _loseNode = scene2::Label::alloc("You lose!", _assets->get<Font>("felt32"));
    _loseNode->setAnchor(Vec2::ANCHOR_CENTER);
    _loseNode->setPosition(dimen.width / 2.0f, dimen.height / 2.0f);
    _loseNode->setForeground(Color4::YELLOW);

    if (_roomID == "") {
        _network.connect();
        _host = true;
    }
    else {
        _network.connect(_roomID);
    }


    _palNode = scene2::AnimationNode::alloc(_assets->get<Texture>("pal_texture"), 4, 21);
    _root->addChild(_palNode);
    _palModel = Pal::alloc(Vec2(-100, 0));
    _palModel->setNode(_palNode);
    _palNode->setPriority(_palModel->getLoc().y);

    _ghostNode = scene2::AnimationNode::alloc(_assets->get<Texture>("ghost_texture"), 4, 21);
    _root->addChild(_ghostNode);
    _ghostModel = Ghost::alloc(Vec2(100, 0));
    _ghostModel->setNode(_ghostNode);
    _ghostNode->setPriority(_ghostModel->getLoc().y);


    if (_host) {
        _player = _palModel;
        _otherPlayers.push_back(_ghostModel);
    }
    else {
        _player =_ghostModel;
        _otherPlayers.push_back(_palModel);
    }
    _networkData->setPlayer(_player);
    _networkData->setOtherPlayers(_otherPlayers);
    
    vector<Vec2> coneShape;
    Vec2 tl(-CONE_WIDTH, CONE_LENGTH);
    Vec2 tr(CONE_WIDTH, CONE_LENGTH);
    Vec2 bl(-CONE_WIDTH / 8, 0);
    Vec2 br(CONE_WIDTH / 8, 0);

    coneShape.push_back(br);
    coneShape.push_back(bl);
    coneShape.push_back(tl);
    coneShape.push_back(tr);

    _visionNode = scene2::PolygonNode::alloc(coneShape);
    _visionNode->setColor(Color4f(1, 1, 1, .2));
    _visionNode->setAnchor(Vec2::ANCHOR_BOTTOM_CENTER);
//    _visionNode->setAngle(M_PI);

    _root->addChild(_visionNode);
    // }

    _player->getNode()->setAnchor(Vec2::ANCHOR_CENTER);
    for (auto& p : _otherPlayers) {
        p->getNode()->setAnchor(Vec2::ANCHOR_CENTER);
    }
    
    return true;
}

/**
*Disposes of all(non - static) resources allocated to this mode.
*/
void GameScene::dispose() {
    if (_active) {
        removeAllChildren();
        _input.dispose();
        _network.dispose();
        _root = nullptr;
        _palNode = nullptr;
        _palModel = nullptr;
        _visionNode = nullptr;
        _ghostNode = nullptr;
        _ghostModel = nullptr;
        _active = false;
    }
}

/**
 * The method called to update the game mode.
 *
 * This method contains any gameplay code that is not an OpenGL call.
 *
 * @param timestep  The amount of time (in seconds) since the last frame
 */
void GameScene::update(float timestep) {
    Size dimen = Application::get()->getDisplaySize();
    dimen *= SCENE_WIDTH / dimen.width;
    Vec2 center(dimen.width / 2, dimen.height / 2);

    // Process input and update player states
    _input.update(timestep);

    Vec2 move = _input.getMove();
    Vec2 direction = _input.getDirection();

    _player->setMove(move);
    _player->setIdle(move == Vec2::ZERO);
    if (direction != Vec2::ZERO) {
        _player->setDir(direction);
    }

    _player->update(timestep);
    updateVision(_player);
    for (auto& p : _otherPlayers) {
        p->update(timestep);
        updateVision(p);
    }

    // Update camera
    _root->setPosition(center - _player->getLoc());

    // Check collisions
    _collision.checkForCollision(_ghostModel, _palModel);
    _collision.checkForCollision(_ghostModel, _visionNode);
    // TODO: Implement checking battery collisions
    // _collision.checkInBounds(_ghostModel, bounds);    need to define bounds


    // If ghost is tagged, lower tag timer
    int tagTimer = _ghostModel->getTimer();
    if (tagTimer > 0) {
        _ghostModel->setTimer(tagTimer - 1);
    }
    if (_ghostModel->getTimer() == 0) {
        _ghostModel->setTagged(false);
    }


    // Pal case
    if (_host) {
        if (_palModel->getSpooked() && _complete == false) {
            // Player loses
            _complete = true;
            _loseNode->setVisible(true);
            _countdown = 120;
        }
        else if (_palModel->getBatteries() == 3 && _complete == false) {
            // Player wins
            _complete = true;
            _winNode->setVisible(true);
            _countdown = 120;
        }

        if (tagTimer == 0) {
            _ghostNode->setVisible(false);
        }
    }
    // Ghost case
    else {
        if (_palModel->getSpooked() && _complete == false) {
            // Player wins
            _complete = true;
            _winNode->setVisible(true);
            _countdown = 120;
        }
        else if (_palModel->getBatteries() == 3 && _complete == false) {
            // Player loses
            _complete = true;
            _loseNode->setVisible(true);
            _countdown = 120;
        }
    }

    // Reset the game if a win condition is reached
    if (_countdown > 0) {
        _countdown--;
    }
    else if (_countdown == 0) {
        reset();
    }
    // Ghost case
    else {
        if (_palModel->getSpooked() && _complete == false) {
            // Player wins
            _complete = true;
            _winNode->setVisible(true);
            _countdown = 120;
        }
        else if (_palModel->getBatteries() == 3 && _complete == false) {
            // Player loses
            _complete = true;
            _loseNode->setVisible(true);
            _countdown = 120;
        }
    }

    // Reset the game if a win condition is reached
    if (_countdown > 0) {
        _countdown--;
    }
    else if (_countdown == 0) {
        reset();
    }

    _network.update(timestep);
}

void GameScene::updateVision(const std::shared_ptr<Player>& player) {
    if (player->getType() != Player::Type::Pal) return;
    
    Vec2 dir = player->getDir();
    // convert degrees to radians
    float angle = atan2(dir.y, dir.x);
    angle = fmod(angle - M_PI_2, 2*M_PI);
    if (angle < 0) angle += 2 * M_PI;

    _visionNode->setAngle(angle);
    _visionNode->setPosition(player->getLoc());
}
