#include "GameScene.h"

using namespace cugl;
using namespace std;

#define CONE_WIDTH 75
#define CONE_LENGTH 200
#define ROOM_SIZE 960

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
    if (!GameMode::init(assets, constants::GameMode::Game)) return false;

    Size dimen = Application::get()->getDisplaySize();
    dimen *= constants::SCENE_WIDTH / dimen.width;
    Rect bounds = Application::get()->getSafeBounds();
    // Background color
    Application::get()->setClearColor(Color4f::BLACK);

    // Init data models
    _networkData = NetworkData::alloc();
    _network->attachData(_networkData);

    _gameMap = GameMap::alloc();
    _gameMap->generateRandomMap();
    _palModel = _gameMap->getPal();
    _ghostModel = _gameMap->getGhost();


    _root = scene2::OrderedNode::allocWithOrder(scene2::OrderedNode::Order::ASCEND);
    //_root->addChild(_assets->get<scene2::SceneNode>("game"));
    _root->setContentSize(dimen);
    _root->doLayout();
    addChild(_root);

    _dimRoot = scene2::OrderedNode::allocWithOrder(scene2::OrderedNode::Order::ASCEND);
    _dimRoot->addChild(_assets->get<scene2::SceneNode>("game"));
    _dimRoot->setContentSize(dimen);
    _dimRoot->doLayout();
    _root->addChild(_dimRoot);

    _litRoot = scene2::OrderedNode::allocWithOrder(scene2::OrderedNode::Order::ASCEND);
    _litRoot->setContentSize(dimen);
    _litRoot->doLayout();
    _root->addChild(_litRoot);

    // Adds the room textures, will probably have to refactor when doing wall collisions
    shared_ptr<Texture> floorTexture = _assets->get<Texture>("dim_floor_texture");
    shared_ptr<Texture> doorTexture = _assets->get<Texture>("dim_door_texture");
    for (auto& room : _gameMap->getRooms()) {
        shared_ptr<scene2::PolygonNode> node = scene2::PolygonNode::allocWithTexture(floorTexture);
        node->setPosition(room->getOrigin());
        node->addChild(scene2::PolygonNode::allocWithTexture(doorTexture));
        _dimRoot->addChild(node);
    }

    // Initialize ending messages
    _ghostWinNode = scene2::Label::alloc("The ghost wins!", _assets->get<Font>("gyparody"));
    _ghostWinNode->setAnchor(Vec2::ANCHOR_CENTER);
    _ghostWinNode->setPosition(dimen.width / 2.0f, dimen.height / 2.0f);
    _ghostWinNode->setForeground(Color4::YELLOW);
    _ghostWinNode->setVisible(false);
    _root->addChild(_ghostWinNode);

    _palWinNode = scene2::Label::alloc("The pals win!", _assets->get<Font>("gyparody"));
    _palWinNode->setAnchor(Vec2::ANCHOR_CENTER);
    _palWinNode->setPosition(dimen.width / 2.0f, dimen.height / 2.0f);
    _palWinNode->setForeground(Color4::YELLOW);
    _palWinNode->setVisible(false);
    _root->addChild(_palWinNode);

    Size shadowSize = Size(160.0, 160.0);

    _assets->get<Texture>("pal_doe_texture")->setName("pal_sprite");
    _palShadowNode = scene2::PolygonNode::allocWithTexture(_assets->get<Texture>("pal_shadow_texture"));
    _palNode = scene2::AnimationNode::alloc(_assets->get<Texture>("pal_doe_texture"), 3, 24);
    _palModel->setNode(_palNode, _palShadowNode);
    _palNode->setPriority(constants::Priority::Player);
    _palShadowNode->setPriority(_palNode->getPriority() - 1);
    _litRoot->addChild(_palNode);
    _palNode->addChild(_palShadowNode);

    //TODO: THESE NEXT TWO LINES MUST BE REPLACED WITH THE NEW GHOST SPRITE TEXTURE INSTEAD OF THE SEAL
    _assets->get<Texture>("ghost_texture")->setName("ghost_sprite");
    _ghostNode = scene2::AnimationNode::alloc(_assets->get<Texture>("ghost_texture"), 3, 24);
    _ghostModel->setNode(_ghostNode);
    _ghostModel->setTimer(0);
    _ghostNode->setPriority(constants::Priority::Player);
    _dimRoot->addChild(_ghostNode);



    if (_network->isHost()) {
        _player = _palModel;
        _otherPlayers.push_back(_ghostModel);
        _gameMap->setPlayer(_palModel);
        _gameMap->setOtherPlayers({ _ghostModel });
    }
    else {
        _player = _ghostModel;
        _otherPlayers.push_back(_palModel);
        _gameMap->setPlayer(_ghostModel);
        _gameMap->setOtherPlayers({ _palModel });
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
    _visionNode->setPriority(constants::Priority::Player);
    
    _root->addChild(_visionNode);

    _player->getNode()->setAnchor(Vec2::ANCHOR_CENTER);
    for (auto& p : _otherPlayers) {
        p->getNode()->setAnchor(Vec2::ANCHOR_CENTER);
    }
    
    return true;
}

/**
 * Disposes of all(non - static) resources allocated to this mode.
 */
void GameScene::dispose() {
    GameMode::dispose();
    setActive(false);

//    _input->dispose();

    _network = nullptr;
    _networkData = nullptr;
    _gameMap = nullptr;
    _root = nullptr;
    _palNode = nullptr;
    _ghostNode = nullptr;
    _visionNode = nullptr;
}

/** Function to sort player node priorities */
bool comparePlayerPriority(const shared_ptr<Player>& p1, const shared_ptr<Player>& p2) {
    return p1->getLoc().y > p2->getLoc().y;
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
    dimen *= constants::SCENE_WIDTH / dimen.width;
    Vec2 center(dimen.width / 2, dimen.height / 2);

    // Process movement input and update player states
    _input->update(timestep);
    _gameMap->move(_input->getMove(), _input->getDirection());
    // Updates vision cones
    updateVision(_player);
    for (auto& p : _otherPlayers) {
        p->update(timestep);
        updateVision(p);
    }

    // Checks if the ghost should be revealed, commented out because no
    // tagging yet
    /**
    if (_player->getType() == Player::Type::Pal) {
        _ghostNode->setVisible(_gameMap->getGhost()->getTagged());
    }
    */

    // Processes interaction input
    _gameMap->update(timestep, _input->getInteraction());

    // Delete after batteries implemented, will be handled in gamemap
    shared_ptr<Trap> trap = nullptr;
    if (_player->getType() == Player::Type::Pal) {
        if (_gameMap->getPal()->getSpooked()) {
            if (_input->getInteraction()) {
                auto slotTexture = _assets->get<Texture>("slot");
                shared_ptr<BatterySlot> slot = BatterySlot::alloc(Vec2(0, 100));
                slot->setTextures(slotTexture, _root->getContentSize());
                slot->getNode()->setScale(0.05f);
                _root->addChild(slot->getNode());
                slot->getNode()->setVisible(true);
                slot->setLoc(_player->getLoc());
                slot->setCharge();
                slot->getNode()->setColor(Color4f::GREEN);
                _gameMap->addSlot(slot);
                //                CULog("ACTIVATE SLOT");
            }
        }
    }
    else {
        if (_input->getInteraction()) {
            if (_gameMap->getTraps().size() != 0) {
                float distance = ROOM_SIZE / 2;
                shared_ptr<Trap> tPtr = nullptr;

                for (shared_ptr<Trap> t : _gameMap->getTraps()) {
                    Vec2 n = t->getLoc() - _player->getLoc();
                    float d = n.length();
                    if (d < distance && !t->getTriggered()) {
                        tPtr = t;
                    }
                }
                trap = tPtr;
            }
            if (trap == nullptr || _gameMap->getTraps().size() == 0) {
                auto trapTexture = _assets->get<Texture>("trap");
                shared_ptr<Trap> t = Trap::alloc(Vec2(0, 0));
                t->setTextures(trapTexture, _root->getContentSize());
                t->getNode()->setScale(0.25f);
                _root->addChild(t->getNode());
                _gameMap->addTrap(t);
                trap = t;
            }

            if (!trap->getArmed() && !trap->getTriggered()) {
                trap->setArmed(true);
                trap->setLoc(_player->getLoc());
            }
            else if (!trap->getTriggered()) {
                trap->setTriggered(true);
                trap->getNode()->setColor(Color4f::RED);
            }
        }
    }

    // Sets priority of player nodes
    vector<shared_ptr<Player>> allPlayers;
    copy(_otherPlayers.begin(), _otherPlayers.end(), back_inserter(allPlayers));
    allPlayers.push_back(_player);
    sort(allPlayers.begin(), allPlayers.end(), &comparePlayerPriority);
    CUAssertLog(allPlayers.size() < constants::PRIORITY_RANGE, "Number of players exceeds priority range.");
    for (unsigned i = 0; i < allPlayers.size(); ++i) {
        allPlayers[i]->getNode()->setPriority(constants::Priority::Player + 1 + i);
    }

    // Update camera
    _root->setPosition(center - _player->getLoc());

    // Check if a player has won
    auto complete = _gameMap->getComplete();
    if (complete[0]) {
        // Pals win
        _palWinNode->setVisible(complete[1]);
        // Ghost wins
        _ghostWinNode->setVisible(!complete[1]);
    }

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

void GameScene::draw(const std::shared_ptr<SpriteBatch>& batch) {
    if (_network->isHost()) {
        

        batch->begin(_camera->getCombined());
        batch->setBlendFunc(_srcFactor, _dstFactor);
        batch->setBlendEquation(_blendEquation);
        _litRoot->render(batch, _root->getNodeToWorldTransform(), _color);
        //_palNode->draw(batch, _root->getNodeToWorldTransform(), _color);
        batch->end();
    }
    else {
        batch->begin(_camera->getCombined());
        batch->setBlendFunc(_srcFactor, _dstFactor);
        batch->setBlendEquation(_blendEquation);
        _root->render(batch, _root->getNodeToWorldTransform(), _color);
        //_litRoot->render(batch, _root->getNodeToWorldTransform(), _color);
        batch->end();
    }
}

void GameScene::shaderDraw(const std::shared_ptr<SpriteBatch>& shaderBatch) {
    if (_network->isHost()) {

        shaderBatch->begin(_camera->getCombined());
        //CULog("pal Model %f, %f", _palModel->getLoc().x, _palModel->getLoc().y);
        //CULog("transformed %f, %f", _root->getPosition().x, _root->getPosition().y);
        GLint uPlayerPos = shaderBatch->getShader()->getUniformLocation("uPlayerPos");
        shaderBatch->getShader()->setUniformVec2(uPlayerPos, _root->getPosition() + _palModel->getLoc());
        GLint uLightAngle = shaderBatch->getShader()->getUniformLocation("uLightAngle");
        shaderBatch->getShader()->setUniform1f(uLightAngle, (_visionNode->getAngle() - M_PI / 2));

        shaderBatch->setBlendFunc(_srcFactor, _dstFactor);

        shaderBatch->setBlendEquation(_blendEquation);
        _dimRoot->render(shaderBatch, _root->getNodeToWorldTransform(), _color);

        shaderBatch->end();
    }
}

void GameScene::reset() {
    auto assets = _assets;
    dispose();
    init(assets);
}
