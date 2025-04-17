#include "LightPass.h"

LightPass::LightPass(ID3D11Device* device, ID3D11DeviceContext* context, ID3D11Texture2D* backBuffer,
	UINT screenWidth, UINT screenHeight, GBuffer* pGBuffer)
	:
	RenderPass("LightPass", device, context)
{

}

void LightPass::StartFrame()
{
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
