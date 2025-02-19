#ifndef SHADERMANAGER_H
#define SHADERMANAGER_H

#include <d3d11.h>
#include <d3dcompiler.h>

#include <iostream>
#include <string>

class ShaderManager
{
public:
    ShaderManager();
    ShaderManager(ID3D11Device* device);
    ~ShaderManager();

    bool LoadVertexShader(LPCWSTR filePath, ID3D11VertexShader** vertexShader, ID3DBlob** shaderBlob);
    bool LoadPixelShader(LPCWSTR filePath, ID3D11PixelShader** pixelShader);

private:
    ID3D11Device* device;
};

#endif // SHADERMANAGER_H

//#ifndef SHADERMANAGER_H
//#define SHADERMANAGER_H
//
//#include <unordered_map>
//#include <string>
//#include <d3d11.h>
//
//class ShaderManager
//{
//public:
//    ShaderManager();
//    ~ShaderManager();
//
//    bool LoadShader(const std::string& name, const std::string& filePath);
//    ID3D11VertexShader* GetVertexShader(const std::string& name);
//    ID3D11PixelShader* GetPixelShader(const std::string& name);
//
//private:
//    std::unordered_map<std::string, ID3D11VertexShader*> vertexShaders;
//    std::unordered_map<std::string, ID3D11PixelShader*> pixelShaders;
//};
//
//#endif // SHADERMANAGER_H