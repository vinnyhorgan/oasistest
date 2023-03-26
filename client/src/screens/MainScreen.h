#pragma once

#include "../ScreenManager.h"
#include "../AssetManager.h"
#include "../Logger.h"
#include "../systems/RenderSystem.h"
#include "../systems/RenderTextSystem.h"

class MainScreen : public Screen
{
public:
    void load() override
    {
        Logger::info("Hello from MainScreen!");

        addSystem<RenderSystem>();
        addSystem<RenderTextSystem>();

        AssetManager::getInstance().loadTexture("player", "assets/viking.png");

        auto entity = registry.create();
        registry.emplace<TransformComponent>(entity, 100, 100);
        registry.emplace<SpriteComponent>(entity, "player", 0, 10.0f, 0.0f);

        auto label = registry.create();
        registry.emplace<TransformComponent>(label, 10, 10);
        registry.emplace<TextComponent>(label, "Hello World!", 20);
    }

    void update(float deltaTime) override
    {

    }

    void render() override
    {
        auto& renderSystem = getSystem<RenderSystem>();
        renderSystem.update(registry, 0);

        auto& renderTextSystem = getSystem<RenderTextSystem>();
        renderTextSystem.update(registry, 0);
    }

    void unload() override
    {

    }
};
