#include "Networking.h"
#include "Logger.h"

#include <enet/enet.h>
#include <imgui.h>

Networking* Networking::instance = nullptr;

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

    ENetAddress address;
    address.host = ENET_HOST_ANY;
    address.port = 1234;

    server = enet_host_create(&address, MAX_CLIENTS, 1, 0, 0);
    if (server == NULL)
    {
        Logger::error("Failed to create ENet server!");
        return;
    }

    Logger::info("Server started on port 1234...");
}

void Networking::update()
{
    ENetEvent event;

    while (enet_host_service(server, &event, 0) > 0)
    {
        switch (event.type)
        {
        case ENET_EVENT_TYPE_CONNECT:
        {
            Logger::info("Client connected!");

            int playerId = 0;
            for (; playerId < MAX_CLIENTS; playerId++)
            {
                if (!players[playerId].active)
                    break;
            }

            std::string message = "Player ID: " + std::to_string(playerId);

            Logger::info(message);

            if (playerId == MAX_CLIENTS)
            {
                Logger::warn("Server is full! Disconnecting client...");
                enet_peer_disconnect(event.peer, 0);
                break;
            }

            players[playerId].active = true;

            players[playerId].validPosition = false;
            players[playerId].peer = event.peer;

            uint8_t acceptBuffer[2];
            acceptBuffer[0] = (uint8_t)NetworkCommands::AcceptPlayer;
            acceptBuffer[1] = (uint8_t)playerId;

            ENetPacket* acceptPacket = enet_packet_create(acceptBuffer, sizeof(acceptBuffer), ENET_PACKET_FLAG_RELIABLE);
            enet_peer_send(event.peer, 0, acceptPacket);

            for (int i = 0; i < MAX_CLIENTS; i++)
            {
                if (i == playerId || !players[i].validPosition)
                    continue;

                uint8_t addBuffer[10];
                addBuffer[0] = (uint8_t)NetworkCommands::AddPlayer;
                addBuffer[1] = (uint8_t)i;
                *(int16_t*)(addBuffer + 2) = (int16_t)players[i].x;
                *(int16_t*)(addBuffer + 4) = (int16_t)players[i].y;
                *(int16_t*)(addBuffer + 6) = (int16_t)players[i].dx;
                *(int16_t*)(addBuffer + 8) = (int16_t)players[i].dy;

                ENetPacket* addPacket = enet_packet_create(addBuffer, sizeof(addBuffer), ENET_PACKET_FLAG_RELIABLE);
                enet_peer_send(event.peer, 0, addPacket);
            }

            break;
        }
        case ENET_EVENT_TYPE_RECEIVE:
        {
            Logger::info("Received packet from client!");

            int playerId = getPlayerId(event.peer);
            if (playerId == -1)
            {
                Logger::warn("Invalid player! Disconnecting client...");
                enet_peer_disconnect(event.peer, 0);
                break;
            }

            int offset = 0;

            NetworkCommands command = (NetworkCommands)readByte(event.packet, offset);

            if (command == UpdateInput)
            {
                players[playerId].x = readShort(event.packet, offset);
                players[playerId].y = readShort(event.packet, offset);
                players[playerId].dx = readShort(event.packet, offset);
                players[playerId].dy = readShort(event.packet, offset);

                NetworkCommands command = UpdatePlayer;

                if (!players[playerId].validPosition)
                    command = AddPlayer;

                players[playerId].validPosition = true;

                uint8_t updateBuffer[10];
                updateBuffer[0] = (uint8_t)command;
                updateBuffer[1] = (uint8_t)playerId;
                *(int16_t*)(updateBuffer + 2) = (int16_t)players[playerId].x;
                *(int16_t*)(updateBuffer + 4) = (int16_t)players[playerId].y;
                *(int16_t*)(updateBuffer + 6) = (int16_t)players[playerId].dx;
                *(int16_t*)(updateBuffer + 8) = (int16_t)players[playerId].dy;

                ENetPacket* updatePacket = enet_packet_create(updateBuffer, sizeof(updateBuffer), ENET_PACKET_FLAG_RELIABLE);
                sendToAllBut(updatePacket, playerId);
            }

            enet_packet_destroy(event.packet);

            break;
        }
        case ENET_EVENT_TYPE_DISCONNECT:
        {
            Logger::info("Client disconnected!");

            int playerId = getPlayerId(event.peer);
            if (playerId == -1)
                break;

            std::string message = "Player ID: " + std::to_string(playerId);

            Logger::info(message);

            players[playerId].active = false;
            players[playerId].peer = nullptr;

            uint8_t removeBuffer[2];
            removeBuffer[0] = (uint8_t)NetworkCommands::RemovePlayer;
            ENetPacket* removePacket = enet_packet_create(removeBuffer, sizeof(removeBuffer), ENET_PACKET_FLAG_RELIABLE);

            sendToAllBut(removePacket, -1);

            break;
        }
        }
    }

    ImGui::Begin("Connected clients");
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (players[i].active)
        {
            ImGui::Text("Client %d", i);
            ImGui::Text("x: %d", players[i].x);
            ImGui::Text("y: %d", players[i].y);
            ImGui::Text("dx: %d", players[i].dx);
            ImGui::Text("dy: %d", players[i].dy);
        }
    }
    ImGui::End();
}

void Networking::disconnect()
{
    enet_host_destroy(server);
    enet_deinitialize();
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

int Networking::getPlayerId(ENetPeer* peer)
{
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (players[i].active && players[i].peer == peer)
            return i;
    }

    return -1;
}

void Networking::sendToAllBut(ENetPacket* packet, int exceptPlayerId)
{
    for (int i = 0; i < MAX_CLIENTS; i++)
    {
        if (!players[i].active || i == exceptPlayerId)
            continue;

        enet_peer_send(players[i].peer, 0, packet);
    }
}
