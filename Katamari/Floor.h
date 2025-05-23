#pragma once
#include <SceneNode.h>

#include "KatamariLighting.h"

//#include <DrawableBase.h>
//class Floor : public DrawableBase<Floor>
class Floor : public SceneNode
{
public:
    Floor();
    Floor(ID3D11Device* device);

    void Update(float deltaTime) override;
    Vector3 GetCenterLocation() override;

    XMFLOAT3 position = XMFLOAT3(0.0f, 0.0f, 0.0f);

    struct Floor_PCB {
        XMMATRIX viewMat;
        XMFLOAT3 cam_pos;
    } floor_pcb;
    Bind::PixelConstantBuffer<Floor_PCB>* pcb;
};

