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
	CreateRandomHeightPlane(50.0f, 50.0f, 50, 50, 0.2f, DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f), &vertices, &verticesNum, &indices, &indicesNum);
	
	worldMat = Matrix::CreateTranslation(position);
	

	AddBind(new Bind::Topology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
	AddBind(new Bind::VertexBuffer(device, vertices, verticesNum, sizeof(CommonVertex)));
	AddBind(new Bind::IndexBuffer(device, indices, indicesNum));
	AddBind(new Bind::TextureB(device, "models\\Textures\\carpet.dds", aiTextureType_DIFFUSE));
	auto vertexShaderB = new Bind::VertexShader(device, L"./Shaders/FloorVShader.hlsl");
	AddBind(vertexShaderB);

	{
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

	}

	AddBind(new Bind::InputLayout(device, IALayoutInputElements, numInputElements, vertexShaderB->GetBytecode()));
	AddBind(new Bind::PixelShader(device, L"./Shaders/FloorPShader.hlsl"));

	AddBind(new Bind::TransformCBuffer(device, this, 0u));

	lightData.dLight = {
		Vector4(0.1f, 0.1f, 0.1f, 0.0f),
		Vector4(0.4f, 0.4f, 0.4f, 0.0f),
		Vector4(0.6f, 0.6f, 0.6f, 0.0f),
		Vector3(-2.0f, -1.0f, -1.0f) / Vector3(-2.0f, -1.0f, -1.0f).Length(),
		0
	};

	for (int i = 0; i < 6; i++)
	{
		/*
		(+1)% 2, // 3, (+2) % 6 // 3
		100
		001
		101
		011
		110
		010
		*/
		lightData.pointLights[i].Ambient = { 0, 0, 0, 1 };
		lightData.pointLights[i].Diffuse = { (i + 1) % 2 * 1.0f, i / 3 * 1.0f, (i + 2) % 6 / 3 * 1.0f, 1 };
		lightData.pointLights[i].Specular = { (i + 1) % 2 * 1.0f, i / 3 * 1.0f, (i + 2) % 6 / 3 * 1.0f, 1 };
		lightData.pointLights[i].Position = { 15.0f * cos(XM_2PI * i / 6.0f), 2.0f, 15.0f * sin(XM_2PI * i / 6.0f) };
		lightData.pointLights[i].Range = 100.0f;
		lightData.pointLights[i].Att = { 0.01f, 1.0f, 0.0f };
		std::cout << (i + 1) % 2 * 1.0f << ", " << i / 3 * 1.0f << ", " << (i + 2) % 6 / 3 * 1.0f << "\n";
	}

	light_pcb = new Bind::PixelConstantBuffer<LightData>(device, lightData, 0u);
	AddBind(light_pcb);
	pcb = new Bind::PixelConstantBuffer<Floor::Floor_PCB>(device, floor_pcb, 1u);
	AddBind(pcb);

	D3D11_RASTERIZER_DESC rastDesc = CD3D11_RASTERIZER_DESC(CD3D11_DEFAULT{});
	rastDesc.CullMode = D3D11_CULL_BACK;
	rastDesc.FillMode = D3D11_FILL_SOLID;
	AddBind(new Bind::Rasterizer(device, rastDesc));
	
	/*
	numInputElements = 3;

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
			D3D11_APPEND_ALIGNED_ELEMENT, // 28,
			D3D11_INPUT_PER_VERTEX_DATA,
			0 };

	vertexShaderFilePath = L"./Shaders/FloorShader.hlsl";
	pixelShaderFilePath = L"./Shaders/FloorShader.hlsl";
	this->textures.push_back(Texture(device, "models\\Textures\\carpet.dds", aiTextureType_DIFFUSE));	
	hasTexture = true;*/
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
