#include "SceneNode.h"

SceneNode::SceneNode()
{
}

SceneNode::~SceneNode()
{
}

//void SceneNode::Update(float deltaTime)
//{
//    // Обновление состояния узла
//}

//void SceneNode::Draw(Microsoft::WRL::ComPtr<ID3D11DeviceContext> context,
//	ID3D11RenderTargetView* renderTargetView)
//{
//    // Отрисовка узла
//}

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