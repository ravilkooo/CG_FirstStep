#include "SceneNode.h"

SceneNode::SceneNode()
{
}

SceneNode::~SceneNode()
{
}


void SceneNode::BindAll(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context)
{
	/*
	// TO-DO: Replace to Bindables .Bind()
	context->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	// TO-DO: Replace to Bindables .Bind()
	for (int i = 0; i < textures.size(); i++)
	{
		if (textures[i].GetType() == aiTextureType_DIFFUSE) {
			//std::cout << "hasTexture_aiTextureType_DIFFUSE\n";
			context->PSSetShaderResources(0, 1, textures[i].GetTextureResourceViewAddress());
			break;
		}
	}

	// TO-DO: Replace to Bindables .Bind()
    context->IASetIndexBuffer(pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// TO-DO: Replace to Bindables .Bind()
	UINT strides[] = { VertexStride() };
	UINT offsets[] = { 0 };
    context->IASetVertexBuffers(0, 1, &(pVertexBuffer), strides, offsets);

	// TO-DO: Replace to Bindables .Bind()
	UpdateCB(context);
	
	// TO-DO: Replace to Bindables .Bind()
	context->PSSetShader(pixelShader, nullptr, 0);
    context->VSSetShader(vertexShader, nullptr, 0);
	*/

	for (size_t i = 0; i < bindables.size(); i++)
	{
		bindables[i]->Bind(context.Get());
	}
}

void SceneNode::Draw(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context) const noexcept {
	context->DrawIndexed(indicesNum, 0, 0);
}


void SceneNode::InitBuffers(ResourceManager resourceManager)
{
	pVertexBuffer = resourceManager.CreateVertexBuffer(vertices, sizeof(CommonVertex) * verticesNum);

	pIndexBuffer = resourceManager.CreateIndexBuffer(indices, sizeof(UINT) * indicesNum);

	pConstantBuffer = resourceManager.CreateConstantBuffer(&cb, sizeof(cb));
}

void SceneNode::LoadAndCompileShader(ShaderManager shaderManager)
{
	if (!shaderManager.LoadVertexShader(vertexShaderFilePath, &vertexShader, &vsBlob))
		std::cout << "Ujas!\n";
	if (!shaderManager.LoadPixelShader(pixelShaderFilePath, &pixelShader))
		std::cout << "Ujas!\n";
}

void SceneNode::UpdateCB(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context)
{
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	context->Map(pConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	memcpy(mappedResource.pData, &cb, sizeof(cb));
	context->Unmap(pConstantBuffer, 0);
	context->VSSetConstantBuffers(0u, 1u, &pConstantBuffer);
}

void SceneNode::InitTextures(std::vector<Texture>& textures)
{
	this->textures = textures;
}

const UINT SceneNode::VertexStride() const
{
	return this->vertexStride;
}

const UINT* SceneNode::VertexStridePtr() const
{
	return &this->vertexStride;
}

std::vector<Texture> SceneNode::LoadMaterialTextures(aiMaterial* pMaterial, aiTextureType textureType, const aiScene* pScene)
{
	std::vector<Texture> materialTextures;
	TextureStorageType storeType = TextureStorageType::Invalid;
	unsigned int textureCount = pMaterial->GetTextureCount(textureType);

	if (textureCount == 0)
	{
		storeType = TextureStorageType::None;
		aiColor3D aiColor(0.0f, 0.0f, 0.0f);
		switch (textureType)
		{
		case aiTextureType_DIFFUSE:
			pMaterial->Get(AI_MATKEY_COLOR_DIFFUSE, aiColor);
			if (aiColor.IsBlack()) //If color = black, just use grey
			{
				materialTextures.push_back(Texture(this->device, SE_Colors::UnloadedTextureColor, textureType));
				return materialTextures;
			}
			materialTextures.push_back(Texture(this->device, SE_Color(aiColor.r * 255, aiColor.g * 255, aiColor.b * 255), textureType));
			return materialTextures;
		}
	}
	else
	{
		for (UINT i = 0; i < textureCount; i++)
		{
			aiString path;
			pMaterial->GetTexture(textureType, i, &path);
			TextureStorageType storeType = DetermineTextureStorageType(pScene, pMaterial, i, textureType);
			switch (storeType)
			{
			case TextureStorageType::Disk:
			{
				std::string filename = this->directory + '\\' + path.C_Str();
				Texture diskTexture(this->device, filename, textureType);
				materialTextures.push_back(diskTexture);
				break;
			}
			}
		}
	}

	if (materialTextures.size() == 0) {
		materialTextures.push_back(Texture(this->device, SE_Colors::UnhandledTextureColor, aiTextureType::aiTextureType_DIFFUSE));
	}
	return materialTextures;
}

TextureStorageType SceneNode::DetermineTextureStorageType(const aiScene* pScene, aiMaterial* pMat, unsigned int index, aiTextureType textureType)
{
	if (pMat->GetTextureCount(textureType) == 0)
		return TextureStorageType::None;

	aiString path;
	pMat->GetTexture(textureType, index, &path);
	std::string texturePath = path.C_Str();
	//Check if texture is an embedded indexed texture by seeing if the file path is an index #
	if (texturePath[0] == '*')
	{
		if (pScene->mTextures[0]->mHeight == 0)
		{
			return TextureStorageType::EmbeddedIndexCompressed;
		}
		else
		{
			assert("SUPPORT DOES NOT EXIST YET FOR INDEXED NON COMPRESSED TEXTURES!" && 0);
			return TextureStorageType::EmbeddedIndexNonCompressed;
		}
	}
	//Check if texture is an embedded texture but not indexed (path will be the texture's name instead of #)
	if (auto pTex = pScene->GetEmbeddedTexture(texturePath.c_str()))
	{
		if (pTex->mHeight == 0)
		{
			return TextureStorageType::EmbeddedCompressed;
		}
		else
		{
			assert("SUPPORT DOES NOT EXIST YET FOR EMBEDDED NON COMPRESSED TEXTURES!" && 0);
			return TextureStorageType::EmbeddedNonCompressed;
		}
	}
	//Lastly check if texture is a filepath by checking for period before extension name
	if (texturePath.find('.') != std::string::npos)
	{
		return TextureStorageType::Disk;
	}

	return TextureStorageType::None; // No texture exists
}

void SceneNode::AddBind(Bind::Bindable* bind)
{
	bindables.push_back(bind);
}

DirectX::XMMATRIX SceneNode::GetViewMatrix() const
{
	return camera->GetViewMatrix();
}

DirectX::XMMATRIX SceneNode::GetProjectionMatrix() const
{
	return camera->GetProjectionMatrix();
}
