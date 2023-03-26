#pragma once

class Game
{
public:
    static int windowWidth;
    static int windowHeight;

    Game() {};
    ~Game() {};

    void run();

private:
    void init();
    void update();
    void render();
    void shutdown();
};
