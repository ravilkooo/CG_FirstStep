#pragma once
#include <wrl.h>
#include <d3d11.h>
#include <directxmath.h>
#include "CommonVertex.h"

class Drawable {
public:
	DirectX::XMMATRIX worldMat = DirectX::XMMatrixIdentity();

	virtual DirectX::XMMATRIX GetViewMatrix() const = 0;
	virtual DirectX::XMMATRIX GetProjectionMatrix() const = 0;

	virtual DirectX::XMFLOAT3 GetCameraPosition() const = 0;

	void Draw(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context) const noexcept;

	CommonVertex* vertices;
	UINT verticesNum;

	int* indices;
	UINT indicesNum;

};