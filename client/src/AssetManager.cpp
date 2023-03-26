#include "AssetManager.h"

AssetManager* AssetManager::instance = nullptr;

AssetManager& AssetManager::getInstance()
{
    if (instance == nullptr)
    {
        instance = new AssetManager();
    }
    return *instance;
}

void AssetManager::loadTexture(const std::string& name, const std::string& path)
{
    Texture2D texture = LoadTexture(path.c_str());
    textures[name] = texture;
}

Texture2D AssetManager::getTexture(const std::string& name)
{
    return textures[name];
}
