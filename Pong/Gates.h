#ifndef GATES_H
#define GATES_H

#include "SceneNode.h"

#include "Ball.h"

#include <DirectXCollision.h>
#include <d3d11.h>

class Gates : public SceneNode
{
public:
	Gates();
	Gates(DirectX::XMFLOAT4 position, float width);
	void Update(float deltaTime);
	void Draw(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context,
		ID3D11RenderTargetView* renderTargetView);

	void HitBall(Ball* ball);
};

#endif // !GATES_H
