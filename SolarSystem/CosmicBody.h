#ifndef COSMICBODY_H
#define COSMICBODY_H

#include <d3d11.h>
#include <directxmath.h>
#include <SceneNode.h>
#include <DirectXMath.h>
#include "SimpleMath.h"
#include "ShapeGenerator.h"

class CosmicBody : public SceneNode
{
public:

    DirectX::XMFLOAT4 position = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);

    CosmicBody(float radius, float rotationSpeed,
        DirectX::XMFLOAT4 position, DirectX::XMFLOAT4 col,
        CosmicBody* attractedTo = nullptr, float orbitRadius = 0.0, float orbitSpeed = 0.0 );
    ~CosmicBody();  

    void Update(float deltaTime) override;

    void SetOrbitSpeed(float speed);
    void SetRotationSpeed(float speed);

    void GetCenterLocation(DirectX::XMFLOAT3* loc);

private:
    float radius;
    float rotationSpeed;  // Скорость вращения вокруг своей оси
    float orbitSpeed;     // Скорость вращения вокруг другого тела
    float orbitRadius;
    CosmicBody* attractedTo;  // Тело, вокруг которого вращается
    float orbitAngle;     // Текущий угол орбиты
    float rotationAngle;
    
    // Текущий угол вращения
};

#endif // COSMICBODY_H