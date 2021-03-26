#include "GameScene.h"
#include "CollisionController.h"

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

    _assets = assets;

    _root = _assets->get<scene2::SceneNode>("game");
    _root->setContentSize(dimen);
    _root->doLayout();
    addChild(_root);

    if (_roomID == "") {
        _network.connect();
        _host = true;
    }
    else {
        _network.connect(_roomID);
    }

    _palNode = dynamic_pointer_cast<scene2::AnimationNode>(_assets->get<scene2::SceneNode>("game_player_pal"));
    //_palNode->setTexture(_assets->get<Texture>("pal_texture"));
    _palModel = Pal::alloc(_palNode->getPosition());
    _palModel->setNode(_palNode);
    
    // VISION CONE
    std::vector<Vec2> coneShape;
    Vec2 tl = Vec2(-CONE_WIDTH, CONE_LENGTH);
    Vec2 tr = Vec2(CONE_WIDTH, CONE_LENGTH);
    Vec2 bl = Vec2(-CONE_WIDTH/8, 0);
    Vec2 br = Vec2(CONE_WIDTH/8, 0);
    
    coneShape.push_back(tl);
    coneShape.push_back(tr);
    coneShape.push_back(br);
    coneShape.push_back(bl);

    _ghostNode = dynamic_pointer_cast<scene2::AnimationNode>(_assets->get<scene2::SceneNode>("game_player_ghost"));
    //_ghostedNode->setTexture(_assets->get<Texture>("ghost_texture"));
    _ghostModel = Ghost::alloc(_ghostNode->getPosition());
    _ghostModel->setNode(_ghostNode);
    

    if (_host) {
        _players.push_back(_palModel);
        _players.push_back(_ghostModel);
    }
    else {
        _players.push_back(_ghostModel);
        _players.push_back(_palModel);
    }
    _networkData->setPlayers(_players);
    
    _visionNode = scene2::PolygonNode::alloc(coneShape);
    _visionNode->setColor(Color4f(1, 1, 1, .2));
    _visionNode->setAnchor(cugl::Vec2::ANCHOR_BOTTOM_CENTER);
    
    _root->addChild(_visionNode);
    
    updateVision(_players[0]);
    
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

    _input.update(timestep);
    _network.update(timestep);

    Vec2 move = _input.getMove();
    Vec2 turning = _input.getTurn();

    shared_ptr<Player> player = _players[0];
    player->move(move);
    if (player->getType() == Player::Type::Pal) {
        dynamic_pointer_cast<Pal>(player)->processTurn(turning);
    }
    _root->setPosition(center - player->getLoc());

    for (auto& p : _players) {
        p->update(timestep);
        updateVision(player);
    }

    // Check win condition
    if (_host) {
        if (_palModel->getSpooked()) {
            // Ghost wins
        }
        else if (_palModel->getBatteries() == 3) {
            // Pal wins
        }
    }
}

void GameScene::updateVision(const std::shared_ptr<Player>& pal) {
    std::string dir = dynamic_pointer_cast<Pal>(pal)->getDirection();
        
    if (dir == "front") {
        _visionNode->setAngle(M_PI);
    } else if (dir == "back") {
        _visionNode->setAngle(0);
    } else if (dir == "right") {
        _visionNode->setAngle(-M_PI_2);
    } else {
        _visionNode->setAngle(M_PI_2);
    }

    _visionNode->setPosition(pal->getLoc());
//    CULog("PLAYER POSITION: %f", _palNode->getPosition());
//    CULog("CONE POSITION: %f", _visionNode->getPosition());
}
