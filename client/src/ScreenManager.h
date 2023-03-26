#pragma once

#include "screens/Screen.h"

#include <entt/entt.hpp>

#include <memory>
#include <vector>

class ScreenManager
{
public:
    static ScreenManager& getInstance();

    void pushScreen(std::unique_ptr<Screen> screen);
    void popScreen();
    void update(float deltaTime);
    void render();

private:
    ScreenManager() {}
    ~ScreenManager() {}

    static ScreenManager* instance;
    std::vector<std::unique_ptr<Screen>> screens;
};
