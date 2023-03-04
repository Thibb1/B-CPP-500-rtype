/*
** EPITECH PROJECT, 2023
** rtype
** File description:
** Entity.cpp
*/

#include "game/Game.hpp"

void Game::updateEntity(Engine::EntityId id) {
    auto [cPosition, cVelocity, cRotation, cCollider] =
        _pObjectArchetype.getComponent<Engine::CPosition, Engine::CVelocity, Engine::CRotation, CCollider>(id);
    cRotation.update();
    cVelocity.update();
    cPosition.addPosition(cVelocity.getVelocity());
    cCollider.setPosition(cPosition.getPosition());
    auto [pModel, pAnimation] = _pObjectArchetype.getComponent<std::shared_ptr<Engine::CModel>, std::shared_ptr<Engine::CAnimation>>(id);
    if (pAnimation != nullptr) {
        pAnimation->update();
        auto animation = pAnimation->getAnimation();
        UpdateModelAnimation(pModel->getModel(), animation, pAnimation->getCurrentFrame());
    }
    if (_pObjectArchetype.getComponent<Engine::CObject>(id).hasTag("emitter")) {
        auto [cObject, cEmitter] = _pObjectArchetype.getComponent<Engine::CObject, CParticleEmitter>(id);

        cEmitter.setPosition(cPosition.getPosition());
        cEmitter.doUpdate();
    }
}

void Game::drawEntity(Engine::EntityId id, Vector3 offset) {
    auto cModel = _pObjectArchetype.getComponent<pModel>(id);
    if (cModel == nullptr)
        return;
    auto position = _pObjectArchetype.getComponent<Engine::CPosition>(id).getPosition();
    auto rotation = _pObjectArchetype.getComponent<Engine::CRotation>(id).getRotation();
    auto scale = _pObjectArchetype.getComponent<Engine::CScale>(id).getScale();
    auto model = cModel->getModel();
    model.transform = MatrixRotateXYZ(rotation);
    auto cObject = _pObjectArchetype.getComponent<Engine::CObject>(id);
    if (cObject.hasTag("emitter")) {
        auto cEmitter = _pObjectArchetype.getComponent<CParticleEmitter>(id);
        cEmitter.draw();
    }
    DrawModel(model, Vector3Add(position, offset), scale, WHITE);
    if (cObject.hasTag("named")) {
        auto text = _pObjectArchetype.getComponent<CText>(id);
        auto textSize = measureText3D(text.getText(), text.getFontSize());
        position.x -= textSize.x * 0.5f;
        rlPushMatrix();
        rlRotatef(90, 1, 0, 0);
        auto textPos = Vector3Add(Vector3Add(position, offset), text.getOffset());
        drawText3D(text.getText(), textPos, text.getFontSize(), WHITE);
        rlPopMatrix();
    }
}

void Game::drawTexture(Engine::EntityId id, Vector3 offset) {
    auto [cObject, cTexture, cAnimatedSprite, cPosition] =
        _pSpriteArchetype.getComponent<Engine::CObject, pTexture, CAnimatedSprite, Engine::CPosition>(id);
    if (cObject.isActive()) {
        // get the texture
        auto texture = cTexture->getTexture();
        // get the source rectangle
        auto source = cAnimatedSprite.getSourceRec();
        // get the scale
        auto scale = cAnimatedSprite.getScale();
        // get the rotation
        auto rotation = cAnimatedSprite.getRotation();
        // get the position
        auto position = cPosition.getPosition();
        position.z += 1;
        // get the camera
        auto camera = _pCameraArchetype.getComponent<CCamera>(_camera).getCamera();
        // draw the billboard pro
        DrawBillboardPro(camera, *texture, source, position, {0, 1, 0}, scale, Vector2Zero(), rotation, WHITE);
    }
}

void Game::addEnemy(int id, Vector3 position, Vector3 velocity, int hp) {
    // auto enemy = _enemies.contains(id) ? _enemies[id] :
    auto enemy = _pObjectArchetype.createEntity(Engine::CScale(0.1), Engine::CPosition(), Engine::CVelocity(), Engine::CObject(),
                                                Engine::CRotation({0, 3, 0}), CHealth(), CCollider());
    auto [cObject, cPosition, cVelocity, cHealth, cCollider] =
        _pObjectArchetype.getComponent<Engine::CObject, Engine::CPosition, Engine::CVelocity, CHealth, CCollider>(enemy);
    if (!cObject.hasTag("emitter")) {
        if (_emitters.contains("enemy_trail")) {
            auto emitter = CParticleEmitter(_emitters["enemy_trail"]);
            emitter.setActive(true);
            _pObjectArchetype.addComponent(enemy, CParticleEmitter(emitter));
            cObject.setTag("emitter");
        }
    }
    cObject.setActive(true);
    cPosition.setPosition(position);
    cVelocity.setVelocity(velocity);
    cVelocity.setActive(true);
    cHealth.setHealth(hp);
    cCollider.setActive(true);
    cCollider.setSize(1);
    cCollider.setPosition(position);
    if (_models.contains("E002")) {
        _pObjectArchetype.addComponent(enemy, pModel(_models["E002"]));
    }
    _enemies[id] = enemy;
}

void Game::addAnimatedSprite(const std::string &name, Vector3 position) {
    if (!_textures.contains(name))
        return;
    auto textureP = _textures[name];
    auto sprite = _pSpriteArchetype.createEntity(Engine::CObject(), Engine::CPosition(), CAnimatedSprite(), pTexture(textureP));
    auto [cObject, cPosition, cAnimatedSprite] = _pSpriteArchetype.getComponent<Engine::CObject, Engine::CPosition, CAnimatedSprite>(sprite);
    cObject.setActive(true);
    cAnimatedSprite.setActive(true);
    cPosition.setPosition(position);
    auto texture = textureP->getTexture();
    cAnimatedSprite.setTextureSize({(float)texture->width, (float)texture->height}, textureP->getRows(), textureP->getColumns());
    cAnimatedSprite.setScale(textureP->getScale());
    _animatedSprites.push_back(sprite);
}

void Game::updateTextures() {
    for (auto it = _animatedSprites.begin(); it != _animatedSprites.end();) {
        auto [cObject, cAnimatedSprite] = _pSpriteArchetype.getComponent<Engine::CObject, CAnimatedSprite>(*it);
        if (!cAnimatedSprite.getEnabled()) {
            it = _animatedSprites.erase(it);
        } else {
            cAnimatedSprite.update();
            ++it;
        }
    }
}
