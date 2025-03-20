#pragma once
#include "SceneNode.h"
#include "StickyBall.h"
#include "ModelLoader.h"
#include <filesystem>
#include "ShapeGenerator.h"
#include <StringHelper.h>
#include <random>
//#include <assimp/ai_assert.h>

class CollectibleObject : public SceneNode
{
public:
    CollectibleObject(ID3D11Device* device, const std::string& modelsFolder, const DirectX::XMFLOAT3& position);
    CollectibleObject(ID3D11Device* device, float radius, const DirectX::XMFLOAT3& position);


    void Update(float deltaTime) override;
    Vector3 GetCenterLocation() override;
    bool CheckCollision(StickyBall& ball);
    void AttachToBall(StickyBall* ball);

    float radius = 1.0f;
    float modelRadius = 0.5f;
    bool isAttached = false;
    float appliedScale = 1.0f;

private:
    static std::vector<std::string> GetModelList(const std::string& folder);
    void LoadRandomModel(const std::string& folder);

    DirectX::XMFLOAT3 initialPosition;
    StickyBall* attachedBall = nullptr;
    Matrix attachTransform;
};