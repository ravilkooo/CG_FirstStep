#pragma once
#include <d3d11.h>
#include <directxmath.h>
#include <SceneNode.h>
#include <DirectXMath.h>
#include "SimpleMath.h"
#include <ShapeGenerator.h>

using namespace DirectX;
using namespace DirectX::SimpleMath;

class GravitationBody : public SceneNode
{
public:
    static constexpr float g_const = 6.67430e-11 * 1e20;
    static constexpr float maxVelocity = 1.0f;
    float mass = 1.0f;

    Vector3 position = Vector3::Zero;
    Vector3 velocity = Vector3::Zero;
    Vector3 acceleration = Vector3::Zero;

    Vector3 CalcForceBetween_noMass(GravitationBody other);

    enum class PLANET_TYPE {
        CUBE = 0, SPHERE = 1, GEOSPHERE = 2, RING = 3,
    };

    GravitationBody(float radius, float spinSpeed,
        XMFLOAT3 position, XMFLOAT4 col, PLANET_TYPE planet_type, float density = 0.1f);
    ~GravitationBody();

    void Update(float deltaTime) override;

    Vector3 GetCenterLocation() override;


    float spinSpeed;  // Скорость вращения вокруг своей оси
    float radius;
    float rotationAngle; // Текущий угол вращения
    Vector3 spinAxis = Vector3(0.0f, 1.0f, 0.0f);

};

