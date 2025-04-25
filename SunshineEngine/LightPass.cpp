#include "LightPass.h"

LightPass::LightPass(ID3D11Device* device, ID3D11DeviceContext* context, ID3D11Texture2D* backBuffer,
	UINT screenWidth, UINT screenHeight, GBuffer* pGBuffer)
	:
	RenderPass("LightPass", device, context)
{
	this->pGBuffer = pGBuffer;

	D3D11_TEXTURE2D_DESC lightDesc = {};
	lightDesc.Width = screenWidth;
	lightDesc.Height = screenHeight;
	lightDesc.MipLevels = 1;
	lightDesc.ArraySize = 1;
	lightDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	lightDesc.SampleDesc.Count = 1;
	lightDesc.Usage = D3D11_USAGE_DEFAULT;
	lightDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	device->CreateTexture2D(&lightDesc, nullptr, pGBuffer->pLightBuffer.GetAddressOf());
	// Normal SRV
	D3D11_SHADER_RESOURCE_VIEW_DESC descSRV = {};
	descSRV.Format = lightDesc.Format;
	descSRV.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	descSRV.Texture2DArray.MostDetailedMip = 0;
	descSRV.Texture2DArray.MipLevels = 1;
	descSRV.Texture2DArray.FirstArraySlice = 0;
	descSRV.Texture2DArray.ArraySize = 1;
	device->CreateShaderResourceView(pGBuffer->pLightBuffer.Get(), &descSRV, pGBuffer->pLightSRV.GetAddressOf());
	// Normal RTV
	HRESULT hr = device->CreateRenderTargetView(pGBuffer->pLightBuffer.Get(), nullptr, pGBuffer->pLightRTV.GetAddressOf());
	if (FAILED(hr))
		throw std::runtime_error("Failed to create Render Target View");

	gBufferRTV = pGBuffer->pLightRTV.Get();

	// Viewport
	viewport = {};
	viewport.Width = static_cast<float>(screenWidth);
	viewport.Height = static_cast<float>(screenHeight);
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.MinDepth = 0;
	viewport.MaxDepth = 1.0f;

	camera = new Camera(screenWidth * 1.0f / screenHeight);
}

void LightPass::StartFrame()
{
	context->OMSetRenderTargets(1, &gBufferRTV, pGBuffer->pDepthDSV.Get());
	float colorBlack[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	context->ClearRenderTargetView(gBufferRTV, colorBlack);
	context->ClearDepthStencilView(pGBuffer->pDepthDSV.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0u);
	context->RSSetViewports(1, &viewport);
}

void LightPass::Pass(const Scene& scene)
{
	BindAllPerFrame();

	for (SceneNode* node : scene.nodes) {
		if (!node->HasTechnique(techniqueTag))
			continue;
		// Turn off depth write
		// Prepare Lights (depthstencil desc and rast desc) and bind (depthstencil and rast)

		node->PassTechnique(techniqueTag, GetDeviceContext());

		// Unbind perobject stuff (depthstencil and rast)
	}
}

void LightPass::EndFrame()
{
	context->PSSetShaderResources(0, NULL, NULL);
	context->OMSetRenderTargets(0, NULL, NULL);
}

Camera* LightPass::GetCamera()
{
	return camera;
}

void LightPass::SetCamera(Camera* camera)
{
	this->camera = camera;
}
