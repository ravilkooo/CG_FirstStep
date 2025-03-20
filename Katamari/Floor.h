#pragma once
#include <SceneNode.h>
class Floor :
    public SceneNode
{
public:
    Floor();
    Floor(ID3D11Device* device);

    void Update(float deltaTime) override;
    Vector3 GetCenterLocation() override;

    XMFLOAT3 position = XMFLOAT3(0.0f, 0.0f, 0.0f);
};

