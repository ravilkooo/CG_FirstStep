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
    bd.Usage = D3D11_USAGE_DYNAMIC;
    bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
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

ID3D11Buffer* ResourceManager::CreateConstantBuffer(UINT size)
{
    D3D11_BUFFER_DESC bd = {};
    bd.Usage = D3D11_USAGE_DYNAMIC;
    bd.ByteWidth = size;
    bd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
    bd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

    ID3D11Buffer* buffer = nullptr;
    device->CreateBuffer(&bd, nullptr, &buffer);
    return buffer;
}