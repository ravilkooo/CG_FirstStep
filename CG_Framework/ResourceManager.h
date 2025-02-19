#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include <d3d11.h>
#include <unordered_map>
#include <string>
#include <wrl.h>

class ResourceManager
{
public:
    Microsoft::WRL::ComPtr<ID3D11Device> device;
    std::unordered_map<std::string, ID3D11Buffer*> buffers;

    ResourceManager();
    ResourceManager(ID3D11Device* device);
    ~ResourceManager();

    ID3D11Buffer* CreateVertexBuffer(const void* data, UINT size);
    ID3D11Buffer* CreateIndexBuffer(const void* data, UINT size);
    ID3D11Buffer* CreateConstantBuffer(UINT size);

private:
};

#endif // RESOURCEMANAGER_H