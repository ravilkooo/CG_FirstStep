#ifndef COSMICBODY_H
#define COSMICBODY_H

#include <d3d11.h>
#include <directxmath.h>
#include <SceneNode.h>
#include <DirectXMath.h>
#include <ShapeGenerator.h>
#include <DrawableBase.h>

using namespace DirectX;
using namespace DirectX::SimpleMath;

class CosmicBody : public DrawableBase<CosmicBody>
{
public:
    enum class PLANET_TYPE {
        CUBE = 0, SPHERE = 1, GEOSPHERE = 2, RING = 3,
    };


    CosmicBody(ID3D11Device* device, float radius, float rotationSpeed,
        XMFLOAT3 position, XMFLOAT4 col, PLANET_TYPE planet_type,
        CosmicBody* attractedTo = nullptr, float orbitRadius = 0.0, float orbitSpeed = 0.0 );
    ~CosmicBody();  

    void Update(float deltaTime) override;

    void SetOrbitSpeed(float speed);
    void SetRotationSpeed(float speed);
    
    Matrix GetAttractedToTransform();
    Vector3 GetCenterLocation() override;

    XMFLOAT3 position = XMFLOAT3(0.0f, 0.0f, 0.0f);

    float rotationSpeed;  // Скорость вращения вокруг своей оси
    float radius;
    float rotationAngle; // Текущий угол вращения
    float orbitAngle;     // Текущий угол орбиты
    Vector3 spinAxis = Vector3(0.0f, 1.0f, 0.0f);
    Vector3 orbitalAxis = Vector3(0.0f, 1.0f, 0.0f);

private:
    float orbitSpeed;     // Скорость вращения вокруг другого тела
    float orbitRadius;
    CosmicBody* attractedTo;  // Тело, вокруг которого вращается
    float eyeTime = 0.0f;


    Bind::VertexShader* vertexShaderB;
   
};

#endif // COSMICBODY_H