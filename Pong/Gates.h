#ifndef GATES_H
#define GATES_H

#include "SceneNode.h"

#include "Ball.h"

#include <DirectXCollision.h>
#include <d3d11.h>

class Gates : public SceneNode
{
public:
	DirectX::XMFLOAT4 position = DirectX::XMFLOAT4(0.0f, 0.0f, 0.0f, 1.0f);
	float width = 0.1;

	Gates();
	Gates(DirectX::XMFLOAT4 position, float width);
	void Update(float deltaTime);
	void Draw(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context,
		ID3D11RenderTargetView* renderTargetView);

	void HitBall(Ball* ball);

	DirectX::BoundingBox GetBoundingBox() const;
};

#endif // !GATES_H
