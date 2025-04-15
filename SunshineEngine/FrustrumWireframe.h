#pragma once
#include "SceneNode.h"
#include "Camera.h"

class FrustrumWireframe :
    public SceneNode
{
public:
    FrustrumWireframe();
    FrustrumWireframe(ID3D11Device* device, Camera frustrumCamera);
    void Update(float deltaTime) override;
    Vector3 GetCenterLocation() override;
    Vector3 position;
    Camera frustrumCamera;

private:
    void FillBuffers();
};

