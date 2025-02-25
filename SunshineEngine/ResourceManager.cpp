#include "ResourceManager.h"

ResourceManager::ResourceManager()
{
}

ResourceManager::ResourceManager(ID3D11Device* device) : device(device)
{
}

ResourceManager::~ResourceManager()
{
    for (auto& buffer : buffers)
    {
        buffer.second->Release();
    }
}

ID3D11Buffer* ResourceManager::CreateVertexBuffer(const void* data, UINT size)
{
    D3D11_BUFFER_DESC bd = {};
    bd.Usage = D3D11_USAGE_DYNAMIC;
    bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    bd.MiscFlags = 0;
    bd.StructureByteStride = 0;
    bd.ByteWidth = size;

    D3D11_SUBRESOURCE_DATA InitData = {};
    InitData.pSysMem = data;
    InitData.SysMemPitch = 0;
    InitData.SysMemSlicePitch = 0;

    ID3D11Buffer* buffer = nullptr;
    device->CreateBuffer(&bd, &InitData, &buffer);
    return buffer;
}

ID3D11Buffer* ResourceManager::CreateIndexBuffer(const void* data, UINT size)
{
    D3D11_BUFFER_DESC bd = {};
    bd.Usage = D3D11_USAGE_DEFAULT;
    bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
    bd.CPUAccessFlags = 0;
    bd.MiscFlags = 0;
    bd.StructureByteStride = 0;
    bd.ByteWidth = size;

    D3D11_SUBRESOURCE_DATA InitData = {};
    InitData.pSysMem = data;
    InitData.SysMemPitch = 0;
    InitData.SysMemSlicePitch = 0;

    ID3D11Buffer* buffer = nullptr;
    device->CreateBuffer(&bd, &InitData, &buffer);
    return buffer;
}

ID3D11Buffer* ResourceManager::CreateConstantBuffer(const void* data, UINT size)
{
    D3D11_BUFFER_DESC bd;
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bd.Usage = D3D11_USAGE_DYNAMIC;
    bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    bd.MiscFlags = 0u;
    bd.ByteWidth = size;
    bd.StructureByteStride = 0u;

    D3D11_SUBRESOURCE_DATA InitData = {};
    InitData.pSysMem = data;

    ID3D11Buffer* buffer = nullptr;
    device->CreateBuffer(&bd, &InitData, &buffer);
    return buffer;
}