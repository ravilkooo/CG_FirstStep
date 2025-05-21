#include "LightPass.h"

LightPass::LightPass(ID3D11Device* device, ID3D11DeviceContext* context,
	ID3D11Texture2D* backBuffer,
	UINT screenWidth, UINT screenHeight, GBuffer* pGBuffer, Camera* camera)
	:
	RenderPass("LightPass", device, context)
{
	this->pGBuffer = pGBuffer;
	this->camera = camera;

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

	camPCB = new Bind::PixelConstantBuffer<CamPCB>(device,
		{ XMMatrixIdentity(), XMMatrixIdentity(),
		(XMFLOAT3)camera->GetPosition(), 0 },
		0u);
	AddPerFrameBind(camPCB);

	D3D11_BLEND_DESC blendDesc = {};
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;

	AddPerFrameBind(new Bind::BlendState(device, blendDesc));

	particleSystem = ParticleSystem(device, context);
	particleSystem.camera = GetCamera();

	// Particle test
#ifdef oldParticlesTest
	{
		{
			ID3DBlob* emitCSBlob = nullptr;
			D3DCompileFromFile(L"./Shaders/EmitCS.hlsl", nullptr, nullptr, "CSMain", "cs_5_0", 0, 0, &emitCSBlob, nullptr);

			device->CreateComputeShader(emitCSBlob->GetBufferPointer(), emitCSBlob->GetBufferSize(), nullptr, &emitComputeShader);


			ID3DBlob* simCSBlob = nullptr;
			D3DCompileFromFile(L"./Shaders/SimulateCS.hlsl", nullptr, nullptr, "CSMain", "cs_5_0", 0, 0, &simCSBlob, nullptr);

			device->CreateComputeShader(simCSBlob->GetBufferPointer(), simCSBlob->GetBufferSize(), nullptr, &simulateComputeShader);


		}


		// Dead List
		std::vector<uint32_t> deadIndices(numParticles);
		for (uint32_t i = 0; i < numParticles; ++i) {
			deadIndices[i] = i;
		}

		D3D11_BUFFER_DESC desc = {};
		desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
		desc.ByteWidth = sizeof(uint32_t) * numParticles;
		desc.StructureByteStride = sizeof(uint32_t);
		desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

		D3D11_SUBRESOURCE_DATA deadInitData = {};
		deadInitData.pSysMem = deadIndices.data();
		device->CreateBuffer(&desc, &deadInitData, &deadList);
		//device->CreateBuffer(&desc, nullptr, &deadList);

		D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
		uavDesc.Format = DXGI_FORMAT_UNKNOWN;
		uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		uavDesc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_APPEND;
		uavDesc.Buffer.NumElements = numParticles;
		device->CreateUnorderedAccessView(deadList, &uavDesc, &uavDeadList);

		D3D11_BUFFER_DESC cbd;
		cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbd.Usage = D3D11_USAGE_DYNAMIC;
		cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		cbd.MiscFlags = 0u;
		cbd.ByteWidth = sizeof(XMFLOAT4) + (16 - (sizeof(XMFLOAT4) % 16)); // aligned size
		cbd.StructureByteStride = 0u;

		device->CreateBuffer(&cbd, nullptr, &emitConstantBuffer);

		/*
		D3D11_MAPPED_SUBRESOURCE mapped;
		GetDeviceContext()->Map(deadList, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
		memcpy(mapped.pData, deadIndices.data(), sizeof(uint32_t) * numParticles);
		GetDeviceContext()->Unmap(deadList, 0);
		*/

		// Sort List
		desc = {};
		desc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
		desc.ByteWidth = 2 * sizeof(uint32_t) * numParticles; //
		desc.StructureByteStride = 2 * sizeof(uint32_t); //
		desc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		device->CreateBuffer(&desc, nullptr, &sortList); // Для sortList: stride = sizeof(uint2)
		
		uavDesc = {};
		uavDesc.Format = DXGI_FORMAT_UNKNOWN;
		uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		uavDesc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_APPEND;
		uavDesc.Buffer.NumElements = numParticles;
		device->CreateUnorderedAccessView(sortList, &uavDesc, &uavSortList);


		// Particle Pool
		particles.resize(numParticles);
		for (size_t i = 0; i < numParticles; i++)
		{
			particles[i] = {
				{3 * cosf(i * XM_2PI * XM_2PI / numParticles), 3 * sinf(i * XM_2PI / numParticles), 0},
				0.05,
				{
					abs(cosf(i * XM_2PI / numParticles)),
					abs(cosf(i * XM_2PI / numParticles + 1)),
					abs(cosf(i * XM_2PI / numParticles + 2)),
					1
				},
			};
		}

		D3D11_BUFFER_DESC particleBufDesc = {};
		particleBufDesc.ByteWidth = sizeof(Particle) * numParticles;
		particleBufDesc.Usage = D3D11_USAGE_DEFAULT;
		particleBufDesc.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
		particleBufDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		particleBufDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;
		particleBufDesc.StructureByteStride = sizeof(Particle);
		GetDevice()->CreateBuffer(&particleBufDesc, nullptr, &particlePool);

		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = DXGI_FORMAT_UNKNOWN;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
		srvDesc.Buffer.NumElements = numParticles;
		GetDevice()->CreateShaderResourceView(particlePool, &srvDesc, &particleSRV);

		uavDesc = {};
		uavDesc.Format = DXGI_FORMAT_UNKNOWN;
		uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		uavDesc.Buffer.NumElements = numParticles;
		device->CreateUnorderedAccessView(particlePool, &uavDesc, &uavParticlePool);

		cbd = {};
		cbd.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		cbd.Usage = D3D11_USAGE_DYNAMIC;
		cbd.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		cbd.MiscFlags = 0u;
		cbd.ByteWidth = sizeof(XMFLOAT4) + (16 - (sizeof(XMFLOAT4) % 16)); // aligned size
		cbd.StructureByteStride = 0u;
		
		Vector3 camPos(camera->GetPosition());
		XMFLOAT4 consts = { camPos.x, camPos.y, camPos.z, 0.016 };

		D3D11_SUBRESOURCE_DATA csInitData = {};
		csInitData.pSysMem = &consts;

		device->CreateBuffer(&cbd, &csInitData, &cs_cb);

		/*
		GetDeviceContext()->Map(particlePool, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
		memcpy(mapped.pData, particles.data(), sizeof(Particle) * numParticles);
		GetDeviceContext()->Unmap(particlePool, 0);
		*/

		struct DrawArgs {
			UINT IndexCountPerInstance;
			UINT InstanceCount;
			UINT StartIndexLocation;
			INT BaseVertexLocation;
			UINT StartInstanceLocation;
		};

		DrawArgs args = {
			6, // Кол-во индексов на частицу (квад)
			numParticles, // Кол-во частиц
			0, 0, 0
		};

		// Создаем буфер для аргументов
		D3D11_BUFFER_DESC argsDesc = {};
		argsDesc.ByteWidth = sizeof(DrawArgs);
		argsDesc.Usage = D3D11_USAGE_DEFAULT;
		argsDesc.BindFlags = 0;
		argsDesc.CPUAccessFlags = 0;
		argsDesc.MiscFlags = D3D11_RESOURCE_MISC_DRAWINDIRECT_ARGS;


		D3D11_SUBRESOURCE_DATA initData = {};
		initData.pSysMem = &args;

		GetDevice()->CreateBuffer(&argsDesc, &initData, &indirectArgsBuffer);

		int indices[6] = {0, 1, 2, 3, 2, 1};
		ps_ib = new Bind::IndexBuffer(GetDevice(), indices, 6);
		ps_vs = new Bind::VertexShader(GetDevice(), L"./Shaders/ParticleVS.hlsl");
		ps_ps = new Bind::PixelShader(GetDevice(), L"./Shaders/ParticleVS.hlsl");
		auto vs_blob = ps_vs->GetBytecode();
		
		D3D11_INPUT_ELEMENT_DESC* IALayoutInputElements = nullptr;
		ps_ia = new Bind::InputLayout(GetDevice(), IALayoutInputElements, 0, vs_blob);
		
		ps_vcb = new Bind::VertexConstantBuffer<XMMATRIX>(GetDevice(), XMMatrixIdentity(), 0u);
	}
#endif
}

void LightPass::StartFrame()
{
	context->OMSetRenderTargets(1, &gBufferRTV, pGBuffer->pDepthDSV.Get());
	float colorBlack[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	context->ClearRenderTargetView(gBufferRTV, colorBlack);
	//context->ClearDepthStencilView(pGBuffer->pDepthDSV.Get(), D3D11_CLEAR_DEPTH, 1.0f, 0u);
	context->RSSetViewports(1, &viewport);

	XMFLOAT3 camPos = camera->GetPosition();
	XMMATRIX vMatInverse = DirectX::XMMatrixTranspose(XMMatrixInverse(nullptr,
		camera->GetViewMatrix()));
	XMMATRIX pMatInverse = DirectX::XMMatrixTranspose(XMMatrixInverse(nullptr,
		camera->GetProjectionMatrix()));
	// camera->GetProjectionMatrix()
	camPCB->Update(GetDeviceContext(), { vMatInverse, pMatInverse, camPos, 0 });
}

void LightPass::Pass(const Scene& scene)
{
	BindAllPerFrame();

	for (SceneNode* node : scene.nodes) {
		if (!node->HasTechnique(techniqueTag))
			continue;
		// Turn off depth write
		// Prepare Lights (depthstencil desc and rast desc) and bind (depthstencil and rast)
		LightObject* lObj = dynamic_cast<LightObject*>(node);
		if (lObj) {
			lObj->UpdateBuffers(context);
			LightObject::LightPosition lightPos = lObj->GetLightPositionInFrustum(GetCamera());

			/*
			if (dynamic_cast<PointLight*>(node)) {
				if (lightPos == LightObject::LightPosition::BEHIND_NEAR_PLANE
					|| lightPos == LightObject::LightPosition::INSIDE
					|| lightPos == LightObject::LightPosition::INTERSECT_FAR_PLANE
					|| lightPos == LightObject::LightPosition::FILL)
				*/
			auto dsDesc = lObj->GetDepthStencilDesc(lightPos);
			auto rastDesc = lObj->GetRasterizerDesc(lightPos);

			ID3D11RasterizerState* rasterState;
			ID3D11DepthStencilState* depthState;
			device->CreateRasterizerState(&rastDesc, &rasterState);
			device->CreateDepthStencilState(&dsDesc, &depthState);

			context->OMSetDepthStencilState(depthState, 0);
			context->RSSetState(rasterState);

			node->PassTechnique(techniqueTag, GetDeviceContext());

			context->OMSetDepthStencilState(nullptr, 0);
			context->RSSetState(nullptr);

		}
		else {
			continue;
		}
	}


	ID3D11ShaderResourceView* nullSRVs[] = { nullptr, nullptr, nullptr, nullptr };
	context->PSSetShaderResources(0, 4, nullSRVs);

	particleSystem.camera = GetCamera();
	particleSystem.Render();

	// Particle test
#ifdef oldParticlesTest
	{
		/*
		currTime += 0.08;
		for (size_t i = 0; i < numParticles; i++)
		{
			particles[i].Position = { 3 * cosf((i + currTime) * XM_2PI * 6 / numParticles + 0.8), 3 * sinf((i + currTime) * XM_2PI / numParticles), 0 };
			particles[i].Size = 0.05 + 0.03 * sinf(i * 0.17 + currTime);
		}
		D3D11_MAPPED_SUBRESOURCE mapped;
		GetDeviceContext()->Map(particlePool, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);
		memcpy(mapped.pData, particles.data(), sizeof(Particle) * numParticles);
		GetDeviceContext()->Unmap(particlePool, 0);
		*/
		{
			// Update cbuffer
			D3D11_MAPPED_SUBRESOURCE mappedResource;
			context->Map(cs_cb, 0u, D3D11_MAP_WRITE_DISCARD, 0u, &mappedResource);
			Vector3 camPos(camera->GetPosition());
			XMFLOAT4 consts = { camPos.x, camPos.y, camPos.z, 0.016 };
			memcpy(mappedResource.pData, &consts, sizeof(consts) + (16 - (sizeof(consts) % 16))); // aligned size
			context->Unmap(cs_cb, 0);
		}
		{
			UINT clearValue[4] = { 0, 0, 0, 0 };
			GetDeviceContext()->ClearUnorderedAccessViewUint(uavSortList, clearValue);

			GetDeviceContext()->CSSetShader(simulateComputeShader, nullptr, 0);
			GetDeviceContext()->CSSetConstantBuffers(0, 1, &cs_cb);

			GetDeviceContext()->CSSetUnorderedAccessViews(0, 1, &uavParticlePool, nullptr);
			GetDeviceContext()->CSSetUnorderedAccessViews(1, 1, &uavDeadList, nullptr);
			GetDeviceContext()->CSSetUnorderedAccessViews(2, 1, &uavSortList, nullptr);
			GetDeviceContext()->Dispatch(numParticles / 64, 1, 1);



			ID3D11UnorderedAccessView* nullUAV[3] = { nullptr, nullptr, nullptr };
			GetDeviceContext()->CSSetUnorderedAccessViews(0, 3, nullUAV, nullptr);
		}
		{

			// Вычисление количества частиц для эмиссии в этом кадре
			int numParticlesToEmit = static_cast<int>(accumulatedTime * emissionRate);
			if (numParticlesToEmit > 0) {
				accumulatedTime -= numParticlesToEmit / emissionRate;
				numParticlesToEmit = min(numParticlesToEmit, 20);

				numNewParticles += numParticlesToEmit;

			}
		}

		if (numNewParticles > 0)
		{
			// Emit
			std::cout << "numNewParticles = " << numNewParticles << "\n";

			EmitConstants consts;
			consts.numParticlesToEmit = numNewParticles;

			// Обновление константного буфера
			D3D11_MAPPED_SUBRESOURCE mappedResource;
			context->Map(emitConstantBuffer, 0u, D3D11_MAP_WRITE_DISCARD, 0u, &mappedResource);
			memcpy(mappedResource.pData, &consts, sizeof(consts) + (16 - (sizeof(consts) % 16))); // aligned size
			context->Unmap(emitConstantBuffer, 0);


			// Привязка буферов
			GetDeviceContext()->CSSetConstantBuffers(0, 1, &emitConstantBuffer);

			GetDeviceContext()->CSSetShader(emitComputeShader, nullptr, 0);

			GetDeviceContext()->CSSetUnorderedAccessViews(0, 1, &uavParticlePool, nullptr);
			GetDeviceContext()->CSSetUnorderedAccessViews(1, 1, &uavDeadList, nullptr);
			GetDeviceContext()->Dispatch(numParticles / 64, 1, 1);

			ID3D11UnorderedAccessView* nullUAV[2] = { nullptr, nullptr };
			GetDeviceContext()->CSSetUnorderedAccessViews(0, 2, nullUAV, nullptr);

			numNewParticles = 0;
		}
		
		ps_ib->Bind(GetDeviceContext());
		ps_vcb->Update(GetDeviceContext(), Matrix::Identity * camera->GetViewMatrix() * camera->GetProjectionMatrix());
		ps_vcb->Bind(GetDeviceContext());
		ps_ia->Bind(GetDeviceContext());
		ps_vs->Bind(GetDeviceContext());
		ps_ps->Bind(GetDeviceContext());

		// Привязываем буфер частиц как SRV
		ID3D11ShaderResourceView* srvs[] = { particleSRV };
		GetDeviceContext()->VSSetShaderResources(4, 1, srvs);

		// Устанавливаем шейдеры, InputLayout и топологию
		GetDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		D3D11_DEPTH_STENCIL_DESC dsDesc = {};
		dsDesc.DepthEnable = TRUE;
		dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
		dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
		D3D11_RASTERIZER_DESC rasterDesc = {};
		rasterDesc.CullMode = D3D11_CULL_NONE;
		rasterDesc.FillMode = D3D11_FILL_SOLID;

		ID3D11RasterizerState* rasterState;
		ID3D11DepthStencilState* depthState;
		device->CreateRasterizerState(&rasterDesc, &rasterState);
		device->CreateDepthStencilState(&dsDesc, &depthState);

		context->OMSetDepthStencilState(depthState, 0);
		context->RSSetState(rasterState);
		
		GetDeviceContext()->DrawIndexedInstancedIndirect(
			indirectArgsBuffer, // Буфер с аргументами
			0                  // Смещение в буфере
		);

		ID3D11ShaderResourceView* nullSRVs[] = { nullptr };
		context->VSSetShaderResources(4, 1, nullSRVs );
		GetDeviceContext()->ClearState();
	}
#endif
}

void LightPass::EndFrame()
{
	//ID3D11ShaderResourceView* nullSRVs[] = { nullptr, nullptr, nullptr, nullptr };

	//context->PSSetShaderResources(0, NULL, NULL);
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
