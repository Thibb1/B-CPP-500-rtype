/*
** EPITECH PROJECT, 2023
** rtype
** File description:
** Game.cpp
*/

#include "game/Game.hpp"

Game::Game() : _ecsManager(nullptr), _udpClient(nullptr), _pObjectArchetype(nullptr) {}

void Game::initGame() {
    BeginDrawing();
    ClearBackground(BLACK);
    EndDrawing();
    rlDisableBackfaceCulling();
    _ecsManager = std::make_shared<ECSManager>();
    _ecsManager->init();
    _pCameraArchetype = _ecsManager->getArchetype<CameraArchetype>("Camera");
    _pObjectArchetype = _ecsManager->getArchetype<ObjectArchetype>("Object");
    _camera = _pCameraArchetype->createEntity(CCamera());
    auto [obj, camera] = _pCameraArchetype->getComponent<Engine::CObject, CCamera>(_camera);
    camera.setCamera(Camera3D{0});
    camera.setPosition({0, 0, 30});
    camera.setTarget(Vector3Zero());
    camera.setUp({0, 1, 0});
    camera.setFovy(20);
    camera.setProjection(CAMERA_PERSPECTIVE);
    SetCameraMode(camera.getCamera(), CAMERA_ORBITAL);
    loadAssets("assets/assets.json");
    loadEntities("assets/levels/level_01.json");
    _pObjectArchetype->getComponent<Engine::CObject>(_gameEntities["R9A1"]).setActive(true);
    _pObjectArchetype->getComponent<Engine::CObject>(_gameEntities["corridor"]).setActive(true);
    for (int i = 0; i < MAX_LIGHTS; i++) {
        _lightIds.insert(i);
    }
    addLight();
    auto shader = *_shaders["lighting"]->getShader();
    auto ambientLoc = GetShaderLocation(shader, "ambient");
    float ambient[4] = {.1, .1, .1, 1};
    SetShaderValue(shader, ambientLoc, ambient, SHADER_UNIFORM_VEC4);
}

void Game::updateGame() {
    _ecsManager->update();
    updateNetwork();
    movePlayer();
    updatePlayer();
    for (auto it = _enemies.begin(); it != _enemies.end();) {
        updateEntity(it->second);
        auto [health, position] = _pObjectArchetype->getComponent<CHealth, Engine::CPosition>(it->second);
        if (health.getHealth() <= 0 || position.getPosition().x < -10) {
            std::cout << "Enemy " << it->first << " died" << std::endl;
            it = _enemies.erase(it);
        } else {
            ++it;
        }
    }
    updateBullets();
    if (_shaders.contains("lighting")) {
        auto playerPosition = _pObjectArchetype->getComponent<Engine::CPosition>(_gameEntities["R9A" + std::to_string(_playerId)]).getPosition();
        playerPosition = Vector3Add(playerPosition, {0, 2, 10});
        _lights[0].setPosition(playerPosition);
    }
    updateLights();
    drawGame();
}

void Game::destroyGame() {
    _ecsManager->destroy();
    _ecsManager = nullptr;
}

void Game::initSocket(const std::string &ip, unsigned short port) { _udpClient = std::make_shared<UdpClient>(ip, port); }

void Game::drawGame() {
    BeginMode3D(_pCameraArchetype->getComponent<CCamera>(_camera).getCamera());
    if (_gameEntities.contains("corridor")) {
        auto corridor = _gameEntities["corridor"];
        auto [cPosition, cObject] = _pObjectArchetype->getComponent<Engine::CPosition, Engine::CObject>(corridor);
        auto position = cPosition.getPosition();
        if (position.x < -7.22) {
            cPosition.setPosition({0, position.y, position.z});
        }
        drawEntity(corridor, {-7.22 * 2, 0, 0});
        drawEntity(corridor, {-7.22, 0, 0});
        drawEntity(corridor, {7.22, 0, 0});
        drawEntity(corridor, {7.22 * 2, 0, 0});
    }
    for (auto &entity : _gameEntities) {
        auto object = _pObjectArchetype->getComponent<Engine::CObject>(entity.second);
        if (!object.isActive())
            continue;
        updateEntity(entity.second);
        drawEntity(entity.second);
    }
    for (auto &enemy : _enemies) {
        drawEntity(enemy.second);
    }
    for (auto &bullet : _bullets) {
        drawEntity(bullet);
    }
    EndMode3D();
    DrawFPS(10, 10);
}
