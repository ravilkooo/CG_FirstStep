#pragma once
#include <d3d11.h>
#include <directxmath.h>

class Drawable {
public:
	DirectX::XMMATRIX worldMat = DirectX::XMMatrixIdentity();

	virtual DirectX::XMMATRIX GetViewMatrix() const = 0;
	virtual DirectX::XMMATRIX GetProjectionMatrix() const = 0;

};