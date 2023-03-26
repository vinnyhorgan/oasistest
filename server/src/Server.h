#pragma once

class Server
{
public:
    static int windowWidth;
    static int windowHeight;

    Server() {};
    ~Server() {};

    void run();

private:
    void init();
    void update();
    void render();
    void shutdown();
};
