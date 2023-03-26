#pragma once

#include "../systems/System.h"

#include <entt/entt.hpp>

#include <memory>
#include <vector>

class Screen
{
public:
    virtual void load() = 0;
    virtual void update(float deltaTime) = 0;
    virtual void render() = 0;
    virtual void unload() = 0;

    template<typename T>
    void addSystem()
    {
        systems.push_back(std::make_unique<T>());
    }

    template<typename T>
    T& getSystem()
    {
        for (auto& system : systems)
        {
            if (typeid(T) == typeid(*system))
            {
                return *static_cast<T*>(system.get());
            }
        }

        throw std::runtime_error("System not found");
    }

protected:
    entt::registry registry;
    std::vector<std::unique_ptr<System>> systems;
};
