#include "Quad.h"

Quad::Quad()
{

	int _ind[6] = { 0, 1, 2, 1, 0, 3 };
	indices = (int *) malloc(6 * sizeof(int));
	indicesNum = 6;
	for (int i = 0; i < 6; i++)
	{
		indices[i] = _ind[i];
	}


	DirectX::XMFLOAT4 _points[8] = {
		DirectX::XMFLOAT4(0.25f, 0.25f, 0.25f, 1.0f),	DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f),
		DirectX::XMFLOAT4(-0.25f, -0.25f, 0.25f, 1.0f),	DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f),
		DirectX::XMFLOAT4(0.25f, -0.25f, 0.25f, 1.0f),	DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f),
		DirectX::XMFLOAT4(-0.25f, 0.25f, 0.25f, 1.0f),	DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
	};
	points = (DirectX::XMFLOAT4*)malloc(8 * sizeof(DirectX::XMFLOAT4));
	pointsNum = 8;
	
	for (int i = 0; i < 8; i++)
	{
		points[i] = _points[i];
	}

	shaderFilePath = L"./Shaders/MyVeryFirstShader.hlsl";
}

Quad::Quad(DirectX::XMFLOAT4 position, float width)
{
	int _ind[6] = { 0, 1, 2, 1, 0, 3 };
	indices = (int*)malloc(6 * sizeof(int));
	indicesNum = 6;
	for (int i = 0; i < 6; i++)
	{
		indices[i] = _ind[i];
	}

	width = width > 0 ? width: 0.01;
	DirectX::XMFLOAT4 _points[8] = {
		DirectX::XMFLOAT4(position.x + width, position.y + width, position.z + 0.0f, position.w + 0.0f),	DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f),
		DirectX::XMFLOAT4(position.x, position.y, position.z + 0.0f, position.w + 0.0f),	DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f),
		DirectX::XMFLOAT4(position.x + width,position.y, position.z + 0.0f, position.w + 0.0f),	DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f),
		DirectX::XMFLOAT4(position.x, position.y + width, position.z + 0.0f, position.w + 0.0f),	DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
	};
	points = (DirectX::XMFLOAT4*)malloc(8 * sizeof(DirectX::XMFLOAT4));
	pointsNum = 8;

	for (int i = 0; i < 8; i++)
	{
		points[i] = _points[i];
	}

	shaderFilePath = L"./Shaders/MyVeryFirstShader.hlsl";
}

Quad::Quad(DirectX::XMFLOAT4 position, float width, float height)
{
	int _ind[6] = { 0, 1, 2, 1, 0, 3 };
	indices = (int*)malloc(6 * sizeof(int));
	indicesNum = 6;
	for (int i = 0; i < 6; i++)
	{
		indices[i] = _ind[i];
	}


	width = width > 0 ? width : 0.01;
	height = height > 0 ? height : 0.01;
	DirectX::XMFLOAT4 _points[8] = {
		DirectX::XMFLOAT4(position.x + width, position.y + height, position.z + 0.0f, position.w + 0.0f),	DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f),
		DirectX::XMFLOAT4(position.x, position.y, position.z + 0.0f, position.w + 0.0f),	DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f),
		DirectX::XMFLOAT4(position.x + width, position.y, position.z + 0.0f, position.w + 0.0f),	DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f),
		DirectX::XMFLOAT4(position.x, position.y + height, position.z + 0.0f, position.w + 0.0f),	DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
	};
	points = (DirectX::XMFLOAT4*)malloc(8 * sizeof(DirectX::XMFLOAT4));
	pointsNum = 8;

	for (int i = 0; i < 8; i++)
	{
		points[i] = _points[i];
	}

	shaderFilePath = L"./Shaders/MyVeryFirstShader.hlsl";
}

Quad::Quad(DirectX::XMFLOAT4* points)
{
	int _ind[6] = { 0, 1, 2, 1, 0, 3 };
	indices = (int*)malloc(6 * sizeof(int));
	indicesNum = 6;
	for (int i = 0; i < 6; i++)
	{
		indices[i] = _ind[i];
	}

	this->points = (DirectX::XMFLOAT4*)malloc(6 * sizeof(DirectX::XMFLOAT4));
	pointsNum = 6;

	for (int i = 0; i < 6; i++)
	{
		this->points[i] = points[i];
	}

	shaderFilePath = L"./Shaders/MyVeryFirstShader.hlsl";
}

void Quad::Update(float deltaTime)
{
}

void Quad::Draw()
{
	/*
	if (!shaderManager.LoadVertexShader(L"./Shaders/MyVeryFirstShader.hlsl", &vertexShader, &vsBlob))
		std::cout << "Ujas!\n";
	if (!shaderManager.LoadPixelShader(L"./Shaders/MyVeryFirstShader.hlsl", &pixelShader))
		std::cout << "Ujas!\n";

	// Input layout handler


	// Описываем какие элементы будут на входе
	D3D11_INPUT_ELEMENT_DESC inputElements[] = {
		D3D11_INPUT_ELEMENT_DESC {
			"POSITION",
			0,
			DXGI_FORMAT_R32G32B32A32_FLOAT,
			0,
			0,
			D3D11_INPUT_PER_VERTEX_DATA,
			0},
		D3D11_INPUT_ELEMENT_DESC {
			"COLOR",
			0,
			DXGI_FORMAT_R32G32B32A32_FLOAT,
			0,
			D3D11_APPEND_ALIGNED_ELEMENT,
			D3D11_INPUT_PER_VERTEX_DATA,
			0}
	};

	inputAssembler.CreateInputLayout(inputElements, vsBlob);

	// 6. Create Set of Points

	// 7. Create Vertex and Index Buffers

	// Create vb
	vb = resourceManager.CreateVertexBuffer(quads[i].points, sizeof(DirectX::XMFLOAT4) * std::size(quads[i].points));

	// Create Set of indices

	// Create ib
	ib = resourceManager.CreateIndexBuffer(quads[i].indices, sizeof(int) * std::size(quads[i].indices));

	//pipelineState.SetRasterizerState(D3D11_CULL_NONE, D3D11_FILL_SOLID);


	D3D11_MAPPED_SUBRESOURCE mappedResource;
	context->Map(vb, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	memcpy(mappedResource.pData, quads[i].points, sizeof(DirectX::XMFLOAT4) * std::size(quads[i].points));
	context->Unmap(vb, 0);

	pipelineState.SetRasterizerState(D3D11_CULL_NONE, D3D11_FILL_SOLID);

	// 8. Setup the IA stage

	// 10. Setup Rasterizer Stage and Viewport

	D3D11_VIEWPORT viewport = {};
	viewport.Width = static_cast<float>(screenWidth);
	viewport.Height = static_cast<float>(screenHeight);
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.MinDepth = 0;
	viewport.MaxDepth = 1.0f;

	context->RSSetViewports(1, &viewport);

	// 8. Setup the IA stage

	inputAssembler.SetInputLayout();
	inputAssembler.SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	context->IASetIndexBuffer(ib, DXGI_FORMAT_R32_UINT, 0);

	UINT strides[] = { 32 };
	UINT offsets[] = { 0 };
	context->IASetVertexBuffers(0, 1, &vb, strides, offsets);

	// 9. Set Vertex and Pixel Shaders

	context->VSSetShader(vertexShader, nullptr, 0);
	context->PSSetShader(pixelShader, nullptr, 0);

	// 11. Set BackBuffer for Output
	context->OMSetRenderTargets(1, &renderTargetView, nullptr);

	// 14. At the End of While (!isExitRequested): Draw the Triangle
	context->DrawIndexed(6, 0, 0);
	//context->Draw(6, 0);

	vertexShader->Release();
	pixelShader->Release();

	if (vb) vb->Release();
	if (ib) ib->Release();
	//inputAssembler.Release();*/
}
