#pragma once

#include "System.h"
#include "../AssetManager.h"
#include "../components/TransformComponent.h"
#include "../components/SpriteComponent.h"

#include <entt/entt.hpp>
#include <raylib.h>

#include <vector>

class RenderSystem : public System
{
public:
    void update(entt::registry& registry, float deltaTime) override
    {
        auto view = registry.view<TransformComponent, SpriteComponent>();

        struct Entity
        {
            TransformComponent transform;
            SpriteComponent sprite;
        };

        std::vector<Entity> entities;

        for (auto entity : view)
        {
            auto& position = view.get<TransformComponent>(entity);
            auto& sprite = view.get<SpriteComponent>(entity);

            // TODO cull entities that are not visible

            entities.push_back({ position, sprite });
        }

        std::sort(entities.begin(), entities.end(), [](const Entity& a, const Entity& b) {
            return a.sprite.zIndex < b.sprite.zIndex;
        });

        for (auto& entity : entities)
        {
            Texture2D texture = AssetManager::getInstance().getTexture(entity.sprite.assetId);
            Vector2 position = { (float)entity.transform.x, (float)entity.transform.y };
            DrawTextureEx(texture, position, entity.sprite.rotation, entity.sprite.scale, WHITE);
        }
    }
};
