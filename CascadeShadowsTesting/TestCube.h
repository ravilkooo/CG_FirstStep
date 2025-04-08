#pragma once
#include <DrawableBase.h>
#include "LightShadowStuff.h"

class TestCube :
    public DrawableBase<TestCube>
{
public:
    TestCube();
    TestCube(ID3D11Device* device);
    TestCube(ID3D11Device* device, float width, float height, float depth, Vector3 position, Vector4 col);

    void Update(float deltaTime) override;

    Vector3 GetCenterLocation() override;

    Vector3 position = Vector3::Zero;

    void SetInitTransform(Matrix transformMat);

private:
    Bind::VertexShader* vertexShaderB = nullptr;
    Matrix initTransform = Matrix::Identity;
};

