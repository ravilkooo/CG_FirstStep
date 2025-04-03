#pragma once
#include <wrl.h>
#include <d3d11.h>
#include <directxmath.h>
#include <SimpleMath.h>
#include <assimp/scene.h>
#include "Camera.h"
#include "Drawable.h"
#include "CommonVertex.h"

namespace Bind {
    class Bindable;
}

class Drawable {
public:
	DirectX::XMMATRIX worldMat = DirectX::XMMatrixIdentity();

    DirectX::XMMATRIX GetViewMatrix() const;
	DirectX::XMMATRIX GetProjectionMatrix() const;

	DirectX::XMFLOAT3 GetCameraPosition() const;

	void Draw(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context) const noexcept;

	CommonVertex* vertices;
	UINT verticesNum;

	int* indices;
	UINT indicesNum;

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

private:
    virtual const std::vector<Bind::Bindable*>& GetStaticBinds() const noexcept = 0;

};