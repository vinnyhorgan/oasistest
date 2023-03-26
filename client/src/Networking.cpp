#include "Networking.h"
#include "Logger.h"

#include <enet/enet.h>

Networking* Networking::instance = nullptr;

ENetHost* client = nullptr;
ENetPeer* server = nullptr;

Networking& Networking::getInstance()
{
    if (instance == nullptr)
    {
        instance = new Networking();
    }
    return *instance;
}

void Networking::connect()
{
    if (enet_initialize() != 0)
    {
        Logger::error("Failed to initialize ENet!");
        return;
    }

    client = enet_host_create(NULL, 1, 1, 0, 0);
    if (client == NULL)
    {
        Logger::error("Failed to create ENet client!");
        return;
    }

    ENetAddress address;
    enet_address_set_host(&address, "localhost");
    address.port = 1234;

    server = enet_host_connect(client, &address, 1, 0);
    if (server == NULL)
    {
        Logger::error("Failed to connect to server!");
        return;
    }
}

void Networking::update(double now, float deltaTime)
{
    lastNow = now;

    if (server == NULL)
        return;

    if (localPlayerId >= 0 && now - lastInputTime > inputUpdateInterval)
    {
        uint8_t updateBuffer[9];
        updateBuffer[0] = (uint8_t)UpdateInput;
        *(int16_t*)(updateBuffer + 1) = (int16_t)players[localPlayerId].position.x;
        *(int16_t*)(updateBuffer + 3) = (int16_t)players[localPlayerId].position.y;
        *(int16_t*)(updateBuffer + 5) = (int16_t)players[localPlayerId].direction.x;
        *(int16_t*)(updateBuffer + 7) = (int16_t)players[localPlayerId].direction.y;

        ENetPacket* packet = enet_packet_create(updateBuffer, sizeof(updateBuffer), ENET_PACKET_FLAG_RELIABLE);
        enet_peer_send(server, 0, packet);

        lastInputTime = now;
    }

    ENetEvent event;

    while (enet_host_service(client, &event, 0) > 0)
    {
        switch (event.type)
        {
        case ENET_EVENT_TYPE_RECEIVE:
        {
            Logger::info("Received packet from server!");

            if (event.packet->dataLength < 1)
                break;

            int offset = 0;

            NetworkCommands command = (NetworkCommands)readByte(event.packet, offset);

            if (localPlayerId == -1)
            {
                if (command == AcceptPlayer)
                {
                    localPlayerId = readByte(event.packet, offset);

                    Logger::info("Accepted by server! Player ID: " + std::to_string(localPlayerId));

                    if (localPlayerId < 0 || localPlayerId >= MAX_CLIENTS)
                    {
                        Logger::error("Invalid player ID!");
                        localPlayerId = -1;
                        break;
                    }

                    lastInputTime = -inputUpdateInterval;

                    players[localPlayerId].active = true;

                    players[localPlayerId].position = Vector2{ 100, 100 };
                }
            }
            else
            {
                switch (command)
                {
                case AddPlayer:
                    handleAddPlayer(event.packet, offset);
                    break;
                case RemovePlayer:
                    handleRemovePlayer(event.packet, offset);
                    break;
                case UpdatePlayer:
                    handleUpdatePlayer(event.packet, offset);
                    break;
                }
            }

            enet_packet_destroy(event.packet);
            break;
        }
        case ENET_EVENT_TYPE_DISCONNECT:
        {
            Logger::info("Disconnected from server!");

            localPlayerId = -1;
            break;
        }
        }

        for (int i = 0; i < MAX_CLIENTS; i++)
        {
            if (i == localPlayerId || !players[i].active)
                continue;

            double delta = lastNow - players[i].updateTime;
            players[i].extrapolatedPosition = Vector2Add(players[i].position, Vector2Scale(players[i].direction, (float)delta));
        }
    }
}

void Networking::disconnect()
{
    enet_peer_disconnect(server, 0);
    enet_host_destroy(client);
    enet_deinitialize();
}

bool Networking::isConnected()
{
    return server != NULL && localPlayerId >= 0;
}

int Networking::getPlayerId()
{
    return localPlayerId;
}

void Networking::updateLocalPlayer(const Vector2& movementDelta, float deltaTime)
{
    if (localPlayerId < 0)
        return;

    players[localPlayerId].position = Vector2Add(players[localPlayerId].position, Vector2Scale(movementDelta, deltaTime));

    players[localPlayerId].direction = movementDelta;
}

uint8_t Networking::readByte(ENetPacket* packet, int& offset)
{
    if (offset > packet->dataLength)
        return 0;

    return packet->data[offset++];
}

int16_t Networking::readShort(ENetPacket* packet, int& offset)
{
    if (offset > packet->dataLength - 2)
        return 0;

    int16_t value = *(int16_t*)(packet->data + offset);
    offset += 2;

    return value;
}

Vector2 Networking::readPosition(ENetPacket* packet, int& offset)
{
    Vector2 pos;
    pos.x = readShort(packet, offset);
    pos.y = readShort(packet, offset);

    return pos;
}

void Networking::handleAddPlayer(ENetPacket* packet, int& offset)
{
    int playerId = readByte(packet, offset);
    if (playerId >= MAX_CLIENTS || playerId == localPlayerId)
        return;

    players[playerId].active = true;
    players[playerId].position = readPosition(packet, offset);
    players[playerId].direction = readPosition(packet, offset);
    players[playerId].updateTime = lastNow;
}

void Networking::handleRemovePlayer(ENetPacket* packet, int& offset)
{
    int playerId = readByte(packet, offset);
    if (playerId >= MAX_CLIENTS || playerId == localPlayerId)
        return;

    players[playerId].active = false;
}

void Networking::handleUpdatePlayer(ENetPacket* packet, int& offset)
{
    int playerId = readByte(packet, offset);
    if (playerId >= MAX_CLIENTS || playerId == localPlayerId || !players[playerId].active)
        return;

    players[playerId].position = readPosition(packet, offset);
    players[playerId].direction = readPosition(packet, offset);
    players[playerId].updateTime = lastNow;
}
