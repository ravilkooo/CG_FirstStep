#pragma once
#include <d3d11.h>
#include <directxmath.h>
#include <SceneNode.h>
#include <DirectXMath.h>
#include "SimpleMath.h"
#include <ShapeGenerator.h>
#include "GravitationBody.h"
#include <DrawableBase.h>

using namespace DirectX;
using namespace DirectX::SimpleMath;

class StarBox : public DrawableBase<StarBox>
{
public:
    Vector3 position = Vector3::Zero;

    StarBox(float radius, float spinSpeed,
        XMFLOAT3 position, XMFLOAT4 col);
    ~StarBox();

    void Update(float deltaTime) override;

    Vector3 GetCenterLocation() override;

    Vector3 ReflectBodyDeltaV(GravitationBody* gBody);

    float spinSpeed;  // Скорость вращения вокруг своей оси
    float radius;
    float rotationAngle; // Текущий угол вращения
    Vector3 spinAxis = Vector3(0.0f, 1.0f, 0.0f);

private:
    Bind::VertexShader* vertexShaderB;
};

