#ifndef TEXTUREMANAGER_H
#define TEXTUREMANAGER_H

#include <unordered_map>
#include <string>
#include <d3d11.h>

class TextureManager
{
public:
    TextureManager();
    ~TextureManager();

    bool LoadTexture(const std::string& name, const std::string& filePath);
    ID3D11ShaderResourceView* GetTexture(const std::string& name);

private:
    std::unordered_map<std::string, ID3D11ShaderResourceView*> textures;
};

#endif // TEXTUREMANAGER_H