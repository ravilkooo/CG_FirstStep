#pragma once
#include <d3d11.h>
#include <directxmath.h>
#include "SE_Color.h"
#include <assimp/material.h>
#include <iostream>


using namespace DirectX;
// using namespace DirectX::SimpleMath;

enum class TextureStorageType
{
	Invalid,
	None,
	EmbeddedIndexCompressed,
	EmbeddedCompressed,
	Disk
};


class Texture
{
public:
	Texture(ID3D11Device* device, const SE_Color & color, aiTextureType type);
	Texture(ID3D11Device* device, const SE_Color * colorData, UINT width, UINT height, aiTextureType type);
	aiTextureType GetType();

	ID3D11ShaderResourceView* GetTextureResourceView();
	ID3D11ShaderResourceView** GetTextureResourceViewAddress();

private:
	void Initialize1x1ColorTexture(ID3D11Device* device, const SE_Color& colorData, aiTextureType type);
	void InitializeColorTexture(ID3D11Device* device, const SE_Color* colorData, UINT width, UINT height, aiTextureType type);

	ID3D11Resource* pTexture = nullptr;
	ID3D11ShaderResourceView* pTextureView = nullptr;
	aiTextureType type = aiTextureType::aiTextureType_UNKNOWN;
};

