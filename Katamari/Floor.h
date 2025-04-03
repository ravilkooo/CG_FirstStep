#pragma once
#include <SceneNode.h>
#include <DrawableBase.h>

#include "KatamariLighting.h"

class Floor : public DrawableBase<Floor>
{
public:
    Floor();
    Floor(ID3D11Device* device);

    void Update(float deltaTime) override;
    Vector3 GetCenterLocation() override;

    XMFLOAT3 position = XMFLOAT3(0.0f, 0.0f, 0.0f);

    struct Floor_PCB {
        XMFLOAT3 cam_pos;
    } floor_pcb;
    Bind::PixelConstantBuffer<Floor_PCB>* pcb;
    Bind::PixelConstantBuffer<LightData>* light_pcb;

    LightData lightData;
};

