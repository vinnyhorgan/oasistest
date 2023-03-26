#include "Game.h"
#include "Logger.h"
#include "ScreenManager.h"
#include "screens/MainScreen.h"
#include "Networking.h"

#include <raylib.h>

int Game::windowWidth = 800;
int Game::windowHeight = 600;

void Game::run()
{
    init();

    while (!WindowShouldClose())
    {
        update();
        render();
    }

    shutdown();
}

void Game::init()
{
    SetTraceLogCallback(Logger::rayLog);
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(windowWidth, windowHeight, "Oasis Client");
    SetTargetFPS(60);

    Logger::setDebug(true);

    ScreenManager::getInstance().pushScreen(std::make_unique<MainScreen>());

    Networking::getInstance().connect();
}

void Game::update()
{
    float dt = GetFrameTime();

    Networking::getInstance().update(GetTime(), dt);

    ScreenManager::getInstance().update(dt);
}

void Game::render()
{
    BeginDrawing();
    ClearBackground(BLACK);

    ScreenManager::getInstance().render();

    EndDrawing();
}

void Game::shutdown()
{
    Networking::getInstance().disconnect();

    CloseWindow();
}
