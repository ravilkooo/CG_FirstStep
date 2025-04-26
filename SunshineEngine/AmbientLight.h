#pragma once
#include "LightObject.h"

class AmbientLight :
    public LightObject
{
public:
    AmbientLight(ID3D11Device* device, Vector4 ambient);

    struct AmbientLightPCB {
        XMFLOAT4 Ambient;
    } AmbientLightData;

    Vector4 ambient;

    Bind::PixelConstantBuffer<AmbientLightPCB>* AmbientLightPBuffer;

    D3D11_DEPTH_STENCIL_DESC GetDepthStencilDesc(LightObject::LightPosition lightPos) override;
    D3D11_RASTERIZER_DESC GetRasterizerDesc(LightObject::LightPosition lightPos) override;

    LightPosition GetLightPositionInFrustum(Camera* camera) override;
    bool IsFrustumInsideOfLight(Camera* camera) override;

    void Update(float deltaTime) override;
    Vector3 GetCenterLocation() override;
};


