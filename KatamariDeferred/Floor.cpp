#include "Floor.h"
#include <ShapeGenerator.h>

Floor::Floor()
{

}

Floor::Floor(ID3D11Device* device)
{
	this->device = device;
	UINT sliceCount = 10;
	UINT elevationCount = 5;
	XMFLOAT4 col(0.9f, 0.0f, 0.0f, 1.0f);
	CreateRandomHeightPlane(100.0f, 100.0f, 4, 4, 0.0f,
		DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f),
		&vertices, &verticesNum, &indices, &indicesNum);
	worldMat = Matrix::CreateTranslation(position);

	// GBufferPass
	{
		RenderTechnique* gBufferPass = new RenderTechnique("GBufferPass");
		gBufferPass->AddBind(new Bind::Topology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
		gBufferPass->AddBind(new Bind::VertexBuffer(device, vertices, verticesNum, sizeof(CommonVertex)));
		gBufferPass->AddBind(new Bind::IndexBuffer(device, indices, indicesNum));
		auto vertexShaderB = new Bind::VertexShader(device, L"./Shaders/GBufferPass/FloorGBufferShaderVS.hlsl");
		gBufferPass->AddBind(vertexShaderB);
		gBufferPass->AddBind(new Bind::TransformCBuffer(device, this, 0u));


		numInputElements = 4;
		IALayoutInputElements = (D3D11_INPUT_ELEMENT_DESC*)malloc(numInputElements * sizeof(D3D11_INPUT_ELEMENT_DESC));

		IALayoutInputElements[0] =
			D3D11_INPUT_ELEMENT_DESC{
				"POSITION",
				0,
				DXGI_FORMAT_R32G32B32_FLOAT,
				0,
				0,
				D3D11_INPUT_PER_VERTEX_DATA,
				0 };
		IALayoutInputElements[1] =
			D3D11_INPUT_ELEMENT_DESC{
				"COLOR",
				0,
				DXGI_FORMAT_R32G32B32A32_FLOAT,
				0,
				D3D11_APPEND_ALIGNED_ELEMENT,
				D3D11_INPUT_PER_VERTEX_DATA,
				0 };
		IALayoutInputElements[2] =
			D3D11_INPUT_ELEMENT_DESC{
				"TEXCOORD",
				0,
				DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT,
				0,
				D3D11_APPEND_ALIGNED_ELEMENT,
				D3D11_INPUT_PER_VERTEX_DATA,
				0 };
		IALayoutInputElements[3] =
			D3D11_INPUT_ELEMENT_DESC{
				"NORMAL",
				0,
				DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT,
				0,
				D3D11_APPEND_ALIGNED_ELEMENT,
				D3D11_INPUT_PER_VERTEX_DATA,
				0 };

		gBufferPass->AddBind(new Bind::InputLayout(device, IALayoutInputElements, numInputElements, vertexShaderB->GetBytecode()));

		gBufferPass->AddBind(new Bind::PixelShader(device, L"./Shaders/GBufferPass/FloorGBufferShaderPS.hlsl"));
		gBufferPass->AddBind(new Bind::TextureB(device, "./models/Textures/carpet.dds", aiTextureType_DIFFUSE, 0u));
		D3D11_SAMPLER_DESC samplerDesc;
		samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.MipLODBias = 0.0f;
		samplerDesc.MaxAnisotropy = 1;
		samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		samplerDesc.BorderColor[0] = 0;
		samplerDesc.BorderColor[1] = 0;
		samplerDesc.BorderColor[2] = 0;
		samplerDesc.BorderColor[3] = 0;
		samplerDesc.MinLOD = 0;
		samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;
		gBufferPass->AddBind(new Bind::Sampler(device, samplerDesc, 0u));

		D3D11_RASTERIZER_DESC rastDesc = CD3D11_RASTERIZER_DESC(CD3D11_DEFAULT{});
		rastDesc.CullMode = D3D11_CULL_BACK;
		rastDesc.FillMode = D3D11_FILL_SOLID;
		gBufferPass->AddBind(new Bind::Rasterizer(device, rastDesc));

		techniques.insert({ "GBufferPass", gBufferPass });
	}
}


void Floor::Update(float deltaTime)
{
	//Matrix viewMat = camera->GetViewMatrix();
	//Matrix projMat = camera->GetProjectionMatrix();


	//cb.wvpMat = worldMat * (XMMATRIX)(viewMat * projMat);
}

Vector3 Floor::GetCenterLocation()
{
	return Vector3::Zero;
}
