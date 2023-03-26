#pragma once

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

struct PlayerInfo
{
    bool active;
    bool validPosition;
    ENetPeer* peer;
    int16_t x;
    int16_t y;
    int16_t dx;
    int16_t dy;
};

class Networking
{
public:
    static Networking& getInstance();

    void connect();
    void update();
    void disconnect();

private:
    Networking() {};
    ~Networking() {};

    static Networking* instance;

    ENetHost* server = nullptr;
    PlayerInfo players[MAX_CLIENTS] = {};

    uint8_t readByte(ENetPacket* packet, int& offset);
    int16_t readShort(ENetPacket* packet, int& offset);
    int getPlayerId(ENetPeer* peer);
    void sendToAllBut(ENetPacket* packet, int exceptPlayerId);
};
