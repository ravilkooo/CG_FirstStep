#pragma once
#include "Bindable.h"
#include "SE_Color.h"
#include <assimp/material.h>
#include "StringHelper.h"
#include <DDSTextureLoader.h>

namespace Bind
{
    class TextureB :
        public Bindable
    {
    public:
        TextureB(ID3D11Device* device, ID3D11Resource* pTexture, ID3D11ShaderResourceView* pTextureView);
        TextureB(ID3D11Device* device, const std::string& filePath, aiTextureType type);
        TextureB(ID3D11Device* device, const SE_Color& color, aiTextureType type);
        TextureB(ID3D11Device* device, const SE_Color* colorData, UINT width, UINT height, aiTextureType type);
        void Bind(ID3D11DeviceContext* context) noexcept override;
        bool HasAlpha() const noexcept;
        aiTextureType GetType();
    private:
        void Initialize1x1ColorTexture(ID3D11Device* device, const SE_Color& colorData, aiTextureType type);
        void InitializeColorTexture(ID3D11Device* device, const SE_Color* colorData, UINT width, UINT height, aiTextureType type);

        std::string filePath;
        bool hasAlpha = false;
        Microsoft::WRL::ComPtr<ID3D11Resource> pTexture;
        Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> pTextureView;
        aiTextureType type = aiTextureType::aiTextureType_UNKNOWN;
    
        //unsigned int slot;
    };

}
