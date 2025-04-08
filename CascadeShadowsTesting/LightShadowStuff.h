#pragma once
#include <d3d11.h>
#include <directxmath.h>

struct LightData {
    struct PointLight
    {
        DirectX::XMFLOAT4 Ambient;
        DirectX::XMFLOAT4 Diffuse;
        DirectX::XMFLOAT4 Specular;
        DirectX::XMFLOAT3 Position;
        float Range;

        DirectX::XMFLOAT3 Att;
        float pad;
    } pointLight;
};

struct ShadowTransform {
    DirectX::XMMATRIX lightView;
    DirectX::XMMATRIX lightProj;
    DirectX::XMMATRIX shadowTransform;
};