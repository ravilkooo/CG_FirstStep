#ifndef BORDER_H
#define BORDER_H

#include "SceneNode.h"
#include "Ball.h"

#include <directxmath.h>
#include <d3d11.h>

class Border : public SceneNode
{
public:
	Border();
	//Border(DirectX::XMFLOAT4 position, float height, float width);
	void Update(float deltaTime);
	void Draw(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context,
		ID3D11RenderTargetView* renderTargetView);

	void HitBall(Ball* ball);
	DirectX::BoundingBox* GetBoundingBoxes() const;
private:
	float height = 2.f;

	DirectX::XMVECTOR normal = { 0.f, 1.f, 0.f, 0.f };
};


#endif // !BORDER_H
