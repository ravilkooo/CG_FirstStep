#include "TextureManager.h"

TextureManager::TextureManager()
{
}

TextureManager::~TextureManager()
{
    for (auto& texture : textures)
    {
        texture.second->Release();
    }
}

bool TextureManager::LoadTexture(const std::string& name, const std::string& filePath)
{
    // Загрузка текстуры
    return true;
}

ID3D11ShaderResourceView* TextureManager::GetTexture(const std::string& name)
{
    return textures[name];
}