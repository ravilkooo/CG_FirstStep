#ifndef QUAD_H
#define QUAD_H

#include "SceneNode.h"

#include <directxmath.h>
#include <d3d11.h>

class Quad : public SceneNode
{
public:
	Quad();
	Quad(DirectX::XMFLOAT4 position, float height);
	Quad(DirectX::XMFLOAT4 position, float height, float width);
	Quad(DirectX::XMFLOAT4* points);
    void Update(float deltaTime);
    void Draw();
};


#endif // !QUAD_H
