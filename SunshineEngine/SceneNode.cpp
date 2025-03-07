#include "SceneNode.h"

SceneNode::SceneNode()
{
}

SceneNode::~SceneNode()
{
}


void SceneNode::Draw(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context,
	ID3D11RenderTargetView* renderTargetView,
	ID3D11DepthStencilView* pDSV)
{
    context->IASetIndexBuffer(pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

    // 6. Create Set of Points
    D3D11_MAPPED_SUBRESOURCE mappedResource;
    context->Map(pConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
    memcpy(mappedResource.pData, &cb, sizeof(cb));
    context->Unmap(pConstantBuffer, 0);

    UINT strides[] = { 32 };
    UINT offsets[] = { 0 };
    context->IASetVertexBuffers(0, 1, &(pVertexBuffer), strides, offsets);

    // 9. Set Vertex and Pixel Shaders
    context->VSSetConstantBuffers(0u, 1u, &pConstantBuffer);

    context->VSSetShader(vertexShader, nullptr, 0);
    context->PSSetShader(pixelShader, nullptr, 0);

    // 11. Set BackBuffer for Output

    context->OMSetRenderTargets(1, &renderTargetView, pDSV);

    // 14. At the End of While (!isExitRequested): Draw the Triangle
    context->DrawIndexed(indicesNum, 0, 0);
}


void SceneNode::InitBuffers(ResourceManager resourceManager)
{
	// 7. Create Vertex and Index Buffers

	// Create pVertexBuffer
	pVertexBuffer = resourceManager.CreateVertexBuffer(points, sizeof(DirectX::XMFLOAT4) * pointsNum);

	// Create Set of indices

	// Create pIndexBuffer
	pIndexBuffer = resourceManager.CreateIndexBuffer(indices, sizeof(int) * indicesNum);

	pConstantBuffer = resourceManager.CreateConstantBuffer(&cb, sizeof(cb));


}

void SceneNode::LoadAndCompileShader(ShaderManager shaderManager)
{
	if (!shaderManager.LoadVertexShader(shaderFilePath, &vertexShader, &vsBlob))
		std::cout << "Ujas!\n";
	if (!shaderManager.LoadPixelShader(shaderFilePath, &pixelShader))
		std::cout << "Ujas!\n";
}

void SceneNode::SetWorldMatrix(const DirectX::XMMATRIX& worldMatrix)
{
	worldMat = worldMatrix;
	cb.wvpMat = worldMat * viewMat * projMat;
}

void SceneNode::SetViewMatrix(const DirectX::XMMATRIX& viewMatrix)
{
	viewMat = viewMatrix;
	cb.wvpMat = worldMat * viewMat * projMat;
}

void SceneNode::SetProjectionMatrix(const DirectX::XMMATRIX& projectionMatrix)
{
	projMat = projectionMatrix;
	cb.wvpMat = worldMat * viewMat * projMat;
}
