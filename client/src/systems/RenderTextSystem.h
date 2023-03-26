#pragma once

#include "System.h"
#include "../components/TransformComponent.h"
#include "../components/TextComponent.h"

#include <entt/entt.hpp>
#include <raylib.h>

#include <vector>

class RenderTextSystem : public System
{
public:
    void update(entt::registry& registry, float deltaTime) override
    {
        auto view = registry.view<TransformComponent, TextComponent>();

        for (auto entity : view)
        {
            auto& position = view.get<TransformComponent>(entity);
            auto& text = view.get<TextComponent>(entity);
            DrawText(text.text.c_str(), position.x, position.y, text.fontSize, WHITE);
        }
    }
};
