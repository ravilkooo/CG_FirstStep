#ifndef TRIANGLE_COMP_H
#define TRIANGLE_COMP_H

#include <d3d11.h>
#include <directxmath.h>

#include "GameComponent.h"

class TriangleComponent : public GameComponent {
private:
	ID3D11InputLayout* layout;

	ID3D11PixelShader* pixelShader;
	// pixelShaderByteCode; // ?
	DirectX::XMFLOAT4* points; // отличие от vertices?

	ID3D11RasterizerState* rastState;

	ID3D11VertexShader* vertexShader;
	// vertexShaderByteCode; // ?
	// vertices;

public:
	TriangleComponent(Game* game);

	void Initialize();

	void Draw();

	void Update();

	void DestroyResources();
};

#endif