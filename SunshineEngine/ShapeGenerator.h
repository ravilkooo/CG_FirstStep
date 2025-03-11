#ifndef SHAPE_GENERATOR_H
#define SHAPE_GENERATOR_H

#include <d3d11.h>
#include <directxmath.h>
#include <iostream>
#include "SceneNode.h"

DirectX::XMVECTOR NormalizeHomogeneousVector(DirectX::XMVECTOR vec);

void CreateSimpleCubeMesh(float width, float height, float depth, DirectX::XMFLOAT4 col,
	Vertex** vertices, UINT* verticesNum, int** indices, UINT* indicesNum);

void CreateSimpleGeosphereMesh(float radius, DirectX::XMFLOAT4 col,
	Vertex** vertices, UINT* verticesNum, int** indices, UINT* indicesNum);

void CreateSimpleSphereMesh(float radius, UINT sliceCount, UINT elevationCount,
	DirectX::XMFLOAT4 col,
	Vertex** vertices, UINT* verticesNum, int** indices, UINT* indicesNum);

void CreateRingMesh(float radius, float thickness, float width,
	UINT sliceCount, DirectX::XMFLOAT4 col,
	Vertex** vertices, UINT* verticesNum, int** indices, UINT* indicesNum);

#endif