#pragma once

#include <raymath.h>

#include <cstdint>

#define MAX_CLIENTS 10

typedef struct _ENetHost ENetHost;
typedef struct _ENetPeer ENetPeer;
typedef struct _ENetEvent ENetEvent;
typedef struct _ENetPacket ENetPacket;

enum NetworkCommands
{
    AcceptPlayer,
    AddPlayer,
    RemovePlayer,
    UpdatePlayer,
    UpdateInput
};

struct RemotePlayer
{
    bool active;
    Vector2 position;
    Vector2 direction;
    double updateTime;
    Vector2 extrapolatedPosition;
};

class Networking
{
public:
    static Networking& getInstance();

    void connect();
    void update(double now, float deltaTime);
    void disconnect();
    bool isConnected();
    int getPlayerId();
    void updateLocalPlayer(const Vector2& movementDelta, float deltaTime);

private:
    Networking() {}
    ~Networking() {}

    static Networking* instance;

    ENetHost* client = nullptr;
    ENetPeer* server = nullptr;
    int localPlayerId = -1;
    RemotePlayer players[MAX_CLIENTS] = {};
    double lastInputTime = -100.0;
    double inputUpdateInterval = 1.0f / 20.0f;
    double lastNow = 0.0;

    uint8_t readByte(ENetPacket* packet, int& offset);
    int16_t readShort(ENetPacket* packet, int& offset);
    Vector2 readPosition(ENetPacket* packet, int& offset);
    void handleAddPlayer(ENetPacket* packet, int& offset);
    void handleRemovePlayer(ENetPacket* packet, int& offset);
    void handleUpdatePlayer(ENetPacket* packet, int& offset);
};
