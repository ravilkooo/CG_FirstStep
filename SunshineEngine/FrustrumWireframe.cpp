#include "FrustrumWireframe.h"

FrustrumWireframe::FrustrumWireframe() {
	
}

FrustrumWireframe::FrustrumWireframe(ID3D11Device* device, Camera frustrumCamera)
	: frustrumCamera(frustrumCamera)
{
	this->device = device;
	this->position = { 0,0,0 };
	this->frustrumCamera = Camera(frustrumCamera);

	// Generate Vertices and indexess
	FillBuffers();

	{
		RenderTechnique* colorPass = new RenderTechnique("MainColorPass");
		colorPass->AddBind(new Bind::Topology(D3D_PRIMITIVE_TOPOLOGY_LINELIST));
		colorPass->AddBind(new Bind::VertexBuffer(device, vertices, verticesNum, sizeof(CommonVertex)));
		colorPass->AddBind(new Bind::IndexBuffer(device, indices, indicesNum));

		Bind::VertexShader* vertexShaderB = new Bind::VertexShader(device, L"./Shaders/WireframeVShader.hlsl");
		colorPass->AddBind(vertexShaderB);

		{
			numInputElements = 1;
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
		}
		colorPass->AddBind(new Bind::InputLayout(device, IALayoutInputElements, numInputElements, vertexShaderB->GetBytecode()));

		colorPass->AddBind(new Bind::PixelShader(device, L"./Shaders/WireframePShader.hlsl"));
		colorPass->AddBind(new Bind::TransformCBuffer(device, this, 0u));
		
		D3D11_RASTERIZER_DESC rastDesc = CD3D11_RASTERIZER_DESC(CD3D11_DEFAULT{});
		rastDesc.CullMode = D3D11_CULL_NONE;
		rastDesc.FillMode = D3D11_FILL_WIREFRAME;
		colorPass->AddBind(new Bind::Rasterizer(device, rastDesc));

		techniques.insert({ "MainColorPass", colorPass });
	}
}

void FrustrumWireframe::Update(float deltaTime) {

}

Vector3 FrustrumWireframe::GetCenterLocation() {
	return position;
}

void FrustrumWireframe::FillBuffers()
{
	verticesNum = 8;
	vertices = (CommonVertex*)calloc(verticesNum, sizeof(CommonVertex));
	UINT vIdx = 0;
	/*
	const Matrix viewProj = frustrumCamera.GetViewMatrix() * frustrumCamera.GetProjectionMatrix();
	const Matrix inv = viewProj.Invert(); // Считаем обратную матрицу вида проекции
	//std::vector<Vector3> frustumCorners; // Не забудьте вызвать reserve 
	for (unsigned int x = 0; x < 2; ++x) {
		for (unsigned int y = 0; y < 2; ++y) {
			for (unsigned int z = 0; z < 2; ++z) {
				const Vector4 pt = Vector4::Transform(
					Vector4(
						2.0f * x - 1.0f, // -1, 1
						2.0f * y - 1.0f, // -1, 1
						z,               //  0, 1
						1.0f), inv);
				//frustumCorners.push_back(Vector3(pt / pt.w));
				vertices[vIdx++] = { Vector3(pt / pt.w) };
			}
		}
	}
	*/
	Vector3 dir = frustrumCamera.GetTarget() - frustrumCamera.GetPosition();
	dir.Normalize();
	Vector3 camUp = frustrumCamera.GetUp();
	Vector3 right = dir.Cross(camUp);
	Vector3 up = right.Cross(dir);

	float sizeWidth = frustrumCamera.GetViewWidth();
	float sizeHeight = frustrumCamera.GetViewHeight();
	float nearZ = frustrumCamera.GetNearZ();
	float farZ = frustrumCamera.GetFarZ();
	Vector3 nearPos = position + dir * nearZ;
	Vector3 farPos = position + dir * farZ;

	Vector3 corner0 = nearPos - right * 0.5 * sizeWidth - up * 0.5 * sizeHeight;
	Vector3 corner1 = farPos - right * 0.5 * sizeWidth - up * 0.5 * sizeHeight;
	Vector3 corner2 = nearPos - right * 0.5 * sizeWidth + up * 0.5 * sizeHeight;
	Vector3 corner3 = farPos - right * 0.5 * sizeWidth + up * 0.5 * sizeHeight;
	Vector3 corner4 = nearPos + right * 0.5 * sizeWidth - up * 0.5 * sizeHeight;
	Vector3 corner5 = farPos + right * 0.5 * sizeWidth - up * 0.5 * sizeHeight;
	Vector3 corner6 = nearPos + right * 0.5 * sizeWidth + up * 0.5 * sizeHeight;
	Vector3 corner7 = farPos + right * 0.5 * sizeWidth + up * 0.5 * sizeHeight;
	
	vertices[vIdx++] = { corner0 };
	vertices[vIdx++] = { corner1 };
	vertices[vIdx++] = { corner2 };
	vertices[vIdx++] = { corner3 };
	vertices[vIdx++] = { corner4 };
	vertices[vIdx++] = { corner5 };
	vertices[vIdx++] = { corner6 };
	vertices[vIdx++] = { corner7 };


	/*
	(-1, -1, 0) 0
	(-1, -1, 1) 1
	(-1, +1, 0) 2
	(-1, +1, 1) 3

	(+1, -1, 0) 4
	(+1, -1, 1) 5
	(+1, +1, 0) 6
	(+1, +1, 1) 7
	*/

	indicesNum = 24;
	indices = (int*)calloc(indicesNum, sizeof(int));

	UINT iIdx = 0;
	{
		indices[iIdx++] = 0;
		indices[iIdx++] = 1;
		
		indices[iIdx++] = 1;
		indices[iIdx++] = 3;

		indices[iIdx++] = 3;
		indices[iIdx++] = 2;

		indices[iIdx++] = 2;
		indices[iIdx++] = 0;

		indices[iIdx++] = 4;
		indices[iIdx++] = 5;

		indices[iIdx++] = 5;
		indices[iIdx++] = 7;

		indices[iIdx++] = 7;
		indices[iIdx++] = 6;

		indices[iIdx++] = 6;
		indices[iIdx++] = 4;

		indices[iIdx++] = 0;
		indices[iIdx++] = 4;

		indices[iIdx++] = 1;
		indices[iIdx++] = 5;

		indices[iIdx++] = 2;
		indices[iIdx++] = 6;

		indices[iIdx++] = 3;
		indices[iIdx++] = 7;
	}


	

}
