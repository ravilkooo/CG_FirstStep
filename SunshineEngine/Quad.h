#ifndef QUAD_H
#define QUAD_H

#include "SceneNode.h"

#include <directxmath.h>
#include <d3d11.h>
#include "CommonVertex.h"

class Quad : public SceneNode<CommonVertex>
{
public:
	Quad();
	Quad(DirectX::XMFLOAT4 position, float height);
	Quad(DirectX::XMFLOAT4 position, float height, float width);
    void Update(float deltaTime);

	Vector3 GetCenterLocation() override;
};


#endif // !QUAD_H
