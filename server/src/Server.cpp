#include "Server.h"
#include "Logger.h"
#include "Networking.h"

#include <raylib.h>
#include <rlImGui.h>

int Server::windowWidth = 800;
int Server::windowHeight = 600;

void Server::run()
{
    init();

    while (!WindowShouldClose())
    {
        update();
        render();
    }

    shutdown();
}

void Server::init()
{
    SetTraceLogCallback(Logger::rayLog);
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);
    InitWindow(windowWidth, windowHeight, "Oasis Server GUI");
    SetTargetFPS(60);

    Logger::setDebug(true);
    Logger::debug("Debug mode enabled!");

    rlImGuiSetup(true);
    Logger::debug("rlImGui initialized!");

    Networking::getInstance().connect();
}

void Server::update()
{

}

void Server::render()
{
    BeginDrawing();
    ClearBackground(BLACK);

    rlImGuiBegin();
    Networking::getInstance().update();
    rlImGuiEnd();

    EndDrawing();
}

void Server::shutdown()
{
    Networking::getInstance().disconnect();
    rlImGuiShutdown();
    CloseWindow();
}
