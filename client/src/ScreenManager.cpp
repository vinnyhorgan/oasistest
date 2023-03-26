#include "ScreenManager.h"

ScreenManager* ScreenManager::instance = nullptr;

ScreenManager& ScreenManager::getInstance()
{
    if (instance == nullptr)
    {
        instance = new ScreenManager();
    }
    return *instance;
}

void ScreenManager::pushScreen(std::unique_ptr<Screen> screen)
{
    screens.push_back(std::move(screen));
    screens.back()->load();
}

void ScreenManager::popScreen()
{
    if (!screens.empty())
    {
        screens.back()->unload();
        screens.pop_back();
    }
}

void ScreenManager::update(float deltaTime)
{
    if (!screens.empty())
    {
        screens.back()->update(deltaTime);
    }
}

void ScreenManager::render()
{
    if (!screens.empty())
    {
        screens.back()->render();
    }
}
