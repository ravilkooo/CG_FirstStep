#pragma once
#include <SceneNode.h>
#include "StickyBall.h"
#include "ModelLoader.h"
#include <filesystem>
#include "ShapeGenerator.h"
#include <StringHelper.h>
#include <random>

// #include <DrawableBase.h>
//class CollectibleObject : public DrawableBase<CollectibleObject>

class CollectibleObject : public SceneNode
{
public:
    CollectibleObject(ID3D11Device* device, const std::string& modelsFolder, const DirectX::XMFLOAT3& position);
    CollectibleObject(ID3D11Device* device, float radius, const DirectX::XMFLOAT3& position);


    void Update(float deltaTime) override;
    Vector3 GetCenterLocation() override;
    bool CheckCollision(StickyBall* ball);
    void AttachToBall(StickyBall* ball);

    float radius = 1.0f;
    float modelRadius = 0.5f;
    bool isAttached = false;
    float appliedScale = 1.0f;
    Matrix randomInitRotation = Matrix::Identity;

    DirectX::XMFLOAT3 initialPosition;
private:
    static std::vector<std::string> GetModelList(const std::string& folder);
    void LoadRandomModel(const std::string& folder);

    StickyBall* attachedBall = nullptr;
    Matrix attachTransform;
    Bind::VertexShader* vertexShaderB;

public:
    struct Collectible_PCB {
        XMFLOAT3 cam_pos;
    } coll_pcb;
    struct Collectible_VCB {
        DirectX::XMFLOAT3 center_position = { 0, 0, 0 };
    } coll_vcb;
    Bind::PixelConstantBuffer<Collectible_PCB>* pcb;
    Bind::VertexConstantBuffer<Collectible_VCB>* vcb;

private:
    Matrix GetRandomRotateTransform() {
        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_real_distribution<float> dis(0.0f, XM_PI * 2);

        float angleX = dis(gen);
        float angleY = dis(gen);
        float angleZ = dis(gen);

        Matrix rotationX = Matrix::CreateRotationX(angleX);
        Matrix rotationY = Matrix::CreateRotationY(angleY);
        Matrix rotationZ = Matrix::CreateRotationZ(angleZ);

        Matrix rotation = rotationX * rotationY * rotationZ;

        return rotation;
    }

};