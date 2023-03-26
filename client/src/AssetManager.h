#pragma once

#include <raylib.h>

#include <map>
#include <string>

class AssetManager
{
public:
    static AssetManager& getInstance();

    void loadTexture(const std::string& name, const std::string& path);
    Texture2D getTexture(const std::string& name);

private:
    AssetManager() {}
    ~AssetManager() {}

    static AssetManager* instance;
    std::map<std::string, Texture2D> textures;
};

