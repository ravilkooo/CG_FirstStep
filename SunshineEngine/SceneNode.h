#ifndef SCENENODE_H
#define SCENENODE_H

#include <d3d11.h>
#include <directxmath.h>
#include <SimpleMath.h>
#include "Camera.h"
#include <assimp/scene.h>
#include "Drawable.h"

#include "BindableCollection.h"


class SceneNode : public Drawable
{
public:
    SceneNode();
    virtual ~SceneNode();

    virtual void Update(float deltaTime) = 0;
    void BindAll(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context);

    virtual Vector3 GetCenterLocation() = 0;

    D3D11_INPUT_ELEMENT_DESC* IALayoutInputElements;
    UINT numInputElements = 2;

    Camera* camera;

    // void AddChild(SceneNode* child) { children.push_back(child); }
    // const std::vector<SceneNode*>& GetChildren() const { return children; }

    ID3D11Device* device;

    void AddBind(Bind::Bindable* bind);
    std::vector<Bind::Bindable*> bindables;

    DirectX::XMMATRIX GetViewMatrix() const override;
    DirectX::XMMATRIX GetProjectionMatrix() const override;
    DirectX::XMFLOAT3 GetCameraPosition() const override;
};

#endif // SCENENODE_H