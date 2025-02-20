#ifndef QUAD_H
#define QUAD_H

#include "SceneNode.h"

#include <directxmath.h>
#include <d3d11.h>

class Quad : public SceneNode
{
public:
    DirectX::XMFLOAT4 points[6];

    void Update(float deltaTime);
    void Draw();

private:
	int indices[6] = { 0,1,2, 1,0,3 };
	DirectX::XMFLOAT4 points[8] = {
		DirectX::XMFLOAT4(0.25f, 0.25f, 0.25f, 1.0f),	DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f),
		DirectX::XMFLOAT4(-0.25f, -0.25f, 0.25f, 1.0f),	DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f),
		DirectX::XMFLOAT4(0.25f, -0.25f, 0.25f, 1.0f),	DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f),
		DirectX::XMFLOAT4(-0.25f, 0.25f, 0.25f, 1.0f),	DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
	};
};

#endif // !QUAD_H
