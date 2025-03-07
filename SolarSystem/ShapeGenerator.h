#ifndef SHAPE_GENERATOR_H
#define SHAPE_GENERATOR_H

#include <d3d11.h>
#include <directxmath.h>
#include <iostream>

void CreateSimpleCubeMesh(float width, float height, float depth,
	DirectX::XMFLOAT4** points, int** indices,
	DirectX::XMFLOAT4 col);

#endif