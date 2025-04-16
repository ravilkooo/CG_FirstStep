#include "GBufferPass.h"

GBufferPass::GBufferPass(ID3D11Device* device, ID3D11DeviceContext* context, ID3D11Texture2D* backBuffer, UINT screenWidth, UINT screenHeight)
	:
	RenderPass("GBufferPass", device, context)
{
	this->backBuffer = backBuffer;
	this->screenWidth = screenWidth;
	this->screenHeight = screenHeight;
	this->gbuffer = new GBuffer();

	// rtv
	HRESULT hr = device->CreateRenderTargetView(backBuffer, nullptr, &renderTargetView);
	if (FAILED(hr))
		throw std::runtime_error("Failed to create Render Target View");

	// --- MRT ---

	// Depth Texture
	D3D11_TEXTURE2D_DESC depthDesc = {};
	depthDesc.Width = screenWidth;
	depthDesc.Height = screenHeight;
	depthDesc.MipLevels = 1;
	depthDesc.ArraySize = 1;
	depthDesc.Format = DXGI_FORMAT_R24G8_TYPELESS;
	depthDesc.SampleDesc.Count = 1;
	depthDesc.SampleDesc.Quality = 0;
	depthDesc.Usage = D3D11_USAGE_DEFAULT;
	depthDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_DEPTH_STENCIL;
	depthDesc.MiscFlags = 0;
	device->CreateTexture2D(&depthDesc, nullptr, gbuffer->pDepthBuffer.GetAddressOf());
	// Depth DSV
	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV = {};
	descDSV.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0u;
	device->CreateDepthStencilView(gbuffer->pDepthBuffer.Get(), &descDSV, gbuffer->pDepthDSV.GetAddressOf());
	// Depth DSV
	D3D11_SHADER_RESOURCE_VIEW_DESC descSRV = {};
	descSRV.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	descSRV.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	descSRV.Texture2DArray.MostDetailedMip = 0;
	descSRV.Texture2DArray.MipLevels = 1;
	descSRV.Texture2DArray.FirstArraySlice = 0;
	descSRV.Texture2DArray.ArraySize = 1;
	device->CreateShaderResourceView(gbuffer->pDepthBuffer.Get(), &descSRV, gbuffer->pDepthSRV.GetAddressOf());

	// Normal Texture
	D3D11_TEXTURE2D_DESC texDesc = {};
	texDesc.Width = screenWidth;
	texDesc.Height = screenHeight;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT; // Normals
	texDesc.SampleDesc.Count = 1;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	device->CreateTexture2D(&texDesc, nullptr, gbuffer->pNormalBuffer.GetAddressOf());
	// Normal RTV
	device->CreateRenderTargetView(gbuffer->pNormalBuffer.Get(), nullptr, gbuffer->pNormalRTV.GetAddressOf());

	// Albedo Texture
	D3D11_TEXTURE2D_DESC texDesc = {};
	texDesc.Width = screenWidth;
	texDesc.Height = screenHeight;
	texDesc.MipLevels = 1;
	texDesc.ArraySize = 1;
	texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // Albedo
	texDesc.SampleDesc.Count = 1;
	texDesc.Usage = D3D11_USAGE_DEFAULT;
	texDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;
	device->CreateTexture2D(&texDesc, nullptr, gbuffer->pAlbedoBuffer.GetAddressOf());
	// Albedo RTV
	device->CreateRenderTargetView(gbuffer->pAlbedoBuffer.Get(), nullptr, gbuffer->pAlbedoRTV.GetAddressOf());

	
	// Set RTVs
	gBufferRTVs[0] = gbuffer->pNormalRTV.Get(); 
	gBufferRTVs[1] = gbuffer->pAlbedoRTV.Get();

	// Viewport
	viewport = {};
	viewport.Width = static_cast<float>(screenWidth);
	viewport.Height = static_cast<float>(screenHeight);
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.MinDepth = 0;
	viewport.MaxDepth = 1.0f;

	camera = Camera(screenWidth * 1.0f / screenHeight);
}

void GBufferPass::StartFrame()
{

	context->OMSetRenderTargets(2, gBufferRTVs, gbuffer->pDepthDSV.Get());
	float color[] = { 0.1f, 0.1f, 0.1f, 1.0f };
	context->ClearRenderTargetView(gBufferRTVs[0], color);
	context->ClearRenderTargetView(gBufferRTVs[1], color);
	context->ClearDepthStencilView(gbuffer->pDepthDSV.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0u);
	context->RSSetViewports(1, &viewport);
}

Camera* GBufferPass::GetCamera()
{
	return &camera;
}

void GBufferPass::SetCamera(Camera* camera)
{
	this->camera = *camera;
}

void GBufferPass::EndFrame()
{
	ID3D11RenderTargetView* nullRTVs[] = { nullptr, nullptr };
	context->OMSetRenderTargets(2, nullRTVs, nullptr);
}
