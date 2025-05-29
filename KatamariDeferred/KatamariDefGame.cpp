#include "KatamariDefGame.h"

KatamariDefGame::KatamariDefGame()
{
	applicationName = L"KatamariDefGame";
	hInstance = GetModuleHandle(nullptr);

	winWidth = 1000;
	winHeight = 800;

	timer = GameTimer();
	scene = Scene();
	physEngine = new PhysicsEngine(&scene);
	displayWindow = DisplayWindow(this, applicationName, hInstance, winWidth, winHeight);
	renderer = new DeferredRenderer(&displayWindow);

	InputDevice::getInstance().OnKeyPressed.AddRaw(this, &KatamariDefGame::HandleKeyDown);
	InputDevice::getInstance().MouseMove.AddRaw(this, &KatamariDefGame::HandleMouseMove);

	GBufferPass* gBufferPass;
	{
		gBufferPass = new GBufferPass(renderer->GetDevice(), renderer->GetDeviceContext(),
			renderer->GetBackBuffer(), winWidth, winHeight);

		renderer->SetMainCamera(gBufferPass->GetCamera());

		renderer->AddPass(gBufferPass);
	}
	{
		gLightPass = new LightPass(renderer->GetDevice(), renderer->GetDeviceContext(),
			renderer->GetBackBuffer(), winWidth, winHeight, gBufferPass->pGBuffer, gBufferPass->GetCamera());


		gLightPass->AddPerFrameBind(new Bind::TextureB(renderer->GetDevice(), gBufferPass->pGBuffer->pNormalSRV.Get(), 0u));
		gLightPass->AddPerFrameBind(new Bind::TextureB(renderer->GetDevice(), gBufferPass->pGBuffer->pAlbedoSRV.Get(), 1u));
		gLightPass->AddPerFrameBind(new Bind::TextureB(renderer->GetDevice(), gBufferPass->pGBuffer->pSpecularSRV.Get(), 2u));
		gLightPass->AddPerFrameBind(new Bind::TextureB(renderer->GetDevice(), gBufferPass->pGBuffer->pWorldPosSRV.Get(), 3u));

		// Usual sampler for all SRV
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

		gLightPass->AddPerFrameBind(new Bind::Sampler(renderer->GetDevice(), samplerDesc, 0u));

		renderer->AddPass(gLightPass);
	}
	// MainColorPass
	{
		MainColorPass* colorPass = new MainColorPass(renderer->GetDevice(), renderer->GetDeviceContext(),
			renderer->GetBackBuffer(), winWidth, winHeight);

		colorPass->SetCamera(renderer->GetMainCamera());

		colorPass->AddPerFrameBind(new Bind::TextureB(renderer->GetDevice(), gBufferPass->pGBuffer->pLightSRV.Get(), 0u));

		// Usual sampler for all SRV
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

		colorPass->AddPerFrameBind(new Bind::Sampler(renderer->GetDevice(), samplerDesc, 0u));

		renderer->AddPass(colorPass);
	}


	floor = new Floor(renderer->GetDevice());
	ball = new StickyBall(renderer->GetDevice());
	renderer->GetMainCamera()->SwitchToFollowMode(ball->position, ball->GetMoveDir(), ball->radius);

	scene.AddNode(floor);
	scene.AddNode(ball);

	SpawnCollectibles();
	
	// Light
	_dl_1 = new DirectionalLight(renderer->GetDevice(), { 5.0f, 5.5f, -5.0f }, { 0.0f, -3.0f, 8.0f },
		//{ 0.0f, 0.0f, 0.0f, 1 }, { 0.0f, 0.0f, 0.0f, 1 }, { 0.0f, 0.0f, 0.0f, 1 });
		{ 0.1f, 0.1f, 0.1f, 1 }, { 0.3f, 0.3f, 0.3f, 1 }, { 0.5f, 0.5f, 0.5f, 1 });
	scene.AddNode(_dl_1);

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<float> dis(0.0f, 1.0f);

	/*
	for (int i = 0; i < _randomPointLights; i++)
	{
		Vector4 col = { dis(gen) * 0.5f + 0.5f, dis(gen) * 0.5f + 0.5f, 1.0f, 1.0f};
		col.z = 2 - col.x - col.y;
		pointLights.push_back(new PointLight(
			renderer->GetDevice(),
			// { 15.0f * cos(XM_2PI * i / 6.0f), 2.0f, 15.0f * sin(XM_2PI * i / 6.0f) },
			{ dis(gen) * 100 - 50, dis(gen) * 100 - 50, dis(gen) * 100 - 50 },
			10.0f,
			{ 0.01f, 1.0f, 0.0f },
			{ 0, 0, 0, 1 },
			// { 0, 0, 0, 1 },
			// { 0, 0, 0, 1 }
			//{ (i + 1) % 2 * 1.0f, i / 3 * 1.0f, (i + 2) % 6 / 3 * 1.0f, 1 },
			//{ (i + 1) % 2 * 1.0f, i / 3 * 1.0f, (i + 2) % 6 / 3 * 1.0f, 1 }
			col, col
		));
		float rotationX = dis(gen) * XM_2PI;
		pointLightSpeeds.push_back(Vector3(cos(rotationX), 0, sin(rotationX)));
		pointLightAccel.push_back(Vector3(sin(rotationX), 0, -cos(rotationX)));

		scene.AddNode(pointLights[i]);
	}
	*/

	PointLight::PointLightPCB* plData =
		(PointLight::PointLightPCB*) calloc(instancedCnt, sizeof(PointLight::PointLightPCB));
	PointLightInstanced::PointLightMovement* movementData =
		(PointLightInstanced::PointLightMovement*)calloc(instancedCnt, sizeof(PointLightInstanced::PointLightMovement));

	for (size_t i = 0; i < instancedCnt; i++)
	{
		plData[i] = {
			{ i * 1.0f / instancedCnt, (i + instancedCnt / 2) % instancedCnt * 1.0f / instancedCnt, 0.0f, 1 },
			{ i * 1.0f / instancedCnt, (i + instancedCnt / 2) % instancedCnt * 1.0f / instancedCnt, 0.0f, 1 },
			{ 3 + 30 * cos(i * XM_2PI / instancedCnt), 2, 3 + 30 * sin(i * XM_2PI / instancedCnt)},
			64,
			{0.01, 1.0f, 0.0f}, 0.0f
		};

		plData[i].Diffuse.z = 2 - plData[i].Diffuse.x - plData[i].Diffuse.y;
		plData[i].Specular.z = 2 - plData[i].Specular.x - plData[i].Specular.y;
		
		movementData[i] = {
			{ cos(i * XM_2PI / instancedCnt), 0, sin(i * XM_2PI / instancedCnt)}, 0,
			{0,0,0}, 0
		};
	}
	instancedPointLights = new PointLightInstanced(renderer->GetDevice(), instancedCnt, plData, movementData, { 0,0,0,1 }, 32);
	scene.AddNode(instancedPointLights);

	/*
	for (int i = 0; i < 8; i++)
	{
		//float _dist = dis(gen) * 2.0f + 5.0f;
		Vector3 lightColor = {
			dis(gen),
			dis(gen),
			dis(gen),
		};

		while (lightColor.Length() < 0.9f) {
			lightColor = {
				dis(gen),
				dis(gen),
				dis(gen)
			};
		}

		pointLights.push_back(new PointLight(
			renderer->GetDevice(),
			{ 0.0f, 0.0f, 0.0f },
			0.0f,
			{ 0.01f, 1.0f, 0.0f },
			{ 0, 0, 0, 1 },
			{ lightColor.x, lightColor.y, lightColor.z, 1.0f },
			{ lightColor.x, lightColor.y, lightColor.z, 1.0f }
		));

		//pointLightPositions[i] = Vector3::Transform({ _dist, 0.0f, 0.0f }, rotation);
		pointLightLifeTime[i] = lifeTime * 2.0f;

		scene.AddNode(pointLights[i + _randomPointLights]);
	}
	*/

	_sl_1 = new SpotLight(renderer->GetDevice(), { 0.0f, 15.0f, 0.0f }, 5.0f, { 0.0f, 1.0f, 0.0f },
		10, { 0.01f, 0.05f, 0.0f },
		{ 0, 0.1, 0, 1 }, { 1, 1, 1, 1 }, { 1, 1, 1, 1 });
	scene.AddNode(_sl_1);

	FullScreenQuad* fsq = new FullScreenQuad(renderer->GetDevice());
	scene.AddNode(fsq);

	for (SceneNode* node : scene.nodes) {
		node->camera = renderer->GetMainCamera();
	}

	//Matrix::CreateFromQuaternion(Quaternion::FromToRotation({ 0,1,0 }, { 0,0,1 }));
	Vector3 emitDir = { 0,0,1 };
	ParticleSystem::EmitterPointConstantBuffer emitterDesc =
	{
		Matrix::CreateFromQuaternion(Quaternion::FromToRotation({ 0,1,0 }, emitDir)),
		{ 0, 0, 0, 1 },
		{ 1, 1, 1, 1 },
		{ 1, 1, 1, 1 },
		100, 4, 10, 1,
		0.5, 0.1,
		0, 3.1415 * 2,
		3.1415 / 3, 20, 0, 0
	};
	ParticleSystem::SimulateParticlesConstantBuffer simulatorDesc = {
		{ 0, -8, 0, 0 }
	};
	dustParticleSystem =
		new ParticleSystem(renderer->GetDevice(), renderer->GetDeviceContext(), emitterDesc, simulatorDesc);
	gLightPass->particleSystems.push_back(dustParticleSystem);
	dustParticleSystem->camera = gLightPass->GetCamera();

	D3D11_BLEND_DESC particleBlendDesc = CD3D11_BLEND_DESC(CD3D11_DEFAULT{});
	particleBlendDesc.RenderTarget[0].BlendEnable = TRUE;
	particleBlendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	particleBlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	particleBlendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	particleBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	particleBlendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
	particleBlendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	particleBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	FLOAT* particleBlendFactor = NULL;
	UINT sampleMask = 0xffffffff;
	dustParticleSystem->SetBlendState(
		new Bind::BlendState(renderer->GetDevice(), particleBlendDesc, particleBlendFactor, sampleMask));

	//new Bind::TextureB(device, "bubbleBC7.dds", aiTextureType_DIFFUSE, 0u);
	dustParticleSystem->SetTexture(
		new Bind::TextureB(renderer->GetDevice(), "dust.dds", aiTextureType_DIFFUSE, 0u));

	dustParticleSystem->SetEmissionRate(0);

	// Stars Particles
	emitterDesc =
	{
		Matrix::Identity,
		{ 0, 0, 0, 1 },
		{ 1, 1, 1, 1 },
		{ 1, 1, 1, 0.5 },
		50, 3, 10, 1,
		0.01, 0.5,
		0, 3.1415 * 2,
		3.1415, 0, 10, 1
	};
	simulatorDesc = {
		{ 0, 0, 0, 0 }
	};
	starParticleSystem =
		new ParticleSystem(renderer->GetDevice(), renderer->GetDeviceContext(), emitterDesc, simulatorDesc);
	gLightPass->particleSystems.push_back(starParticleSystem);
	starParticleSystem->camera = gLightPass->GetCamera();

	particleBlendDesc = CD3D11_BLEND_DESC(CD3D11_DEFAULT{});
	particleBlendDesc.RenderTarget[0].BlendEnable = TRUE;
	particleBlendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	particleBlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	particleBlendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	particleBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	particleBlendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
	particleBlendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	particleBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	starParticleSystem->SetBlendState(
		new Bind::BlendState(renderer->GetDevice(), particleBlendDesc, particleBlendFactor, sampleMask));
	starParticleSystem->SetTexture(
		new Bind::TextureB(renderer->GetDevice(), "primogem.dds", aiTextureType_DIFFUSE, 0u));
	starParticleSystem->SetEmissionRate(0);

	// Bubble Particles
	emitterDesc =
	{
		Matrix::Identity,
		{ 0, 40, 0, 1 },
		{ 1, 1, 1, 1 },
		{ 1, 1, 1, 0 },
		100, 8, 8, 1,
		0.5, 0.5,
		0, 3.1415 * 2,
		3.1415 / 10, 0, 0, 0
	};
	simulatorDesc = {
		{ 0, -5, 0, 0 }
	};
	bubbleParticleSystem =
		new ParticleSystem(renderer->GetDevice(), renderer->GetDeviceContext(), emitterDesc, simulatorDesc);
	gLightPass->particleSystems.push_back(bubbleParticleSystem);
	bubbleParticleSystem->camera = gLightPass->GetCamera();

	particleBlendDesc = CD3D11_BLEND_DESC(CD3D11_DEFAULT{});
	particleBlendDesc.RenderTarget[0].BlendEnable = TRUE;
	particleBlendDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	particleBlendDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	particleBlendDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	particleBlendDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	particleBlendDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
	particleBlendDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	particleBlendDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	bubbleParticleSystem->SetBlendState(
		new Bind::BlendState(renderer->GetDevice(), particleBlendDesc, particleBlendFactor, sampleMask));
	bubbleParticleSystem->SetTexture(
		new Bind::TextureB(renderer->GetDevice(), "bubble24bpp.dds", aiTextureType_DIFFUSE, 0u));
	bubbleParticleSystem->SetEmissionRate(40);

	
	normalMap = new Bind::TextureB(renderer->GetDevice(), gBufferPass->pGBuffer->pNormalSRV.Get(), 0u, Bind::PipelineStage::COMPUTE_SHADER);
	worldPosMap = new Bind::TextureB(renderer->GetDevice(), gBufferPass->pGBuffer->pWorldPosSRV.Get(), 1u, Bind::PipelineStage::COMPUTE_SHADER);

	sam = new Bind::Sampler(renderer->GetDevice(), CD3D11_SAMPLER_DESC(), 0u, Bind::PipelineStage::COMPUTE_SHADER);
	particlesViewProjMat = new Bind::ComputeConstantBuffer<viewProjectionBuffer>(renderer->GetDevice(), 3u);

	for (auto partSys : gLightPass->particleSystems)
	{
		partSys->AddBindablesToSimulationPass(normalMap);
		partSys->AddBindablesToSimulationPass(worldPosMap);
		partSys->AddBindablesToSimulationPass(sam);
		partSys->AddBindablesToSimulationPass(particlesViewProjMat);
	}


}

KatamariDefGame::~KatamariDefGame()
{
}

void KatamariDefGame::Update(float deltaTime)
{
	particlesViewProjMat->Update(renderer->GetDeviceContext(),
		{ renderer->GetMainCamera()->GetViewMatrix(), renderer->GetMainCamera()->GetProjectionMatrix() });

	//std::cout << floor->binds.size() << " " << ball->binds.size() << " " << "\n";
	// delete this

	ball->SlowDown(deltaTime);
	physEngine->Update(deltaTime);

	// particle test
	Vector3 dustEmitDir = ball->GetMoveDir() +  Vector3(0,1,0);
	dustEmitDir.Normalize();
	Vector4 dustEmitPos = Vector4(ball->GetCenterLocation());
	dustEmitPos.y = 0;
	dustParticleSystem->SetEmitDir(dustEmitDir);
	dustParticleSystem->SetEmitPosition(dustEmitPos);
	dustParticleSystem->SetEmissionRate(40 * sqrt(ball->velocity / ball->maxVelocity));
	dustParticleSystem->Update(deltaTime);
	//gLightPass->accumulatedTime += deltaTime;


	//std::cout << sqrt(ball->radiusGrow * sqrt(ball->radius) * 2) << "\n";
	starParticleSystem->SetEmitPosition(Vector4(ball->GetCenterLocation()));
	starParticleSystem->SetEmissionRate(100 * sqrt(ball->radiusGrow * sqrt(ball->radius) * 2));
	starParticleSystem->Update(deltaTime);

	bubbleFlowDirection = Vector3::Transform(bubbleFlowDirection, Matrix::CreateRotationY(10 * deltaTime));
	bubbleParticleSystem->SetEmitDir(bubbleFlowDirection);
	bubbleParticleSystem->Update(deltaTime);

	// Проверка коллизий
	for (auto coll : collectibles)
	{
		if (coll->CheckCollision(ball))
		{
			coll->AttachToBall(ball);
			ball->Grow(coll->radius / deltaTime);
		}
	}
	renderer->GetMainCamera()->Update(deltaTime, ball->worldMat, ball->GetMoveDir());

	//Matrix vpMat = renderer->camera.GetViewMatrix() * renderer->camera.GetProjectionMatrix();


	XMFLOAT3 camera_pos = renderer->GetMainCamera()->GetPosition();
	for (auto coll : collectibles)
	{
		coll->vcb->Update(renderer->GetDeviceContext(),
			{
				coll->isAttached ? ball->position : coll->initialPosition
			}
		);
	}

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<float> dis(0.0f, 1.0f);
	/*
	for (size_t i = 0; i < _randomPointLights; i++)
	{

		float rotationX = dis(gen) * XM_2PI;
		//float rotationY = dis(gen) * XM_PI;

		Vector3 da = Vector3(cos(rotationX), 0, sin(rotationX));
		pointLightAccel[i] += da * deltaTime;
		pointLightAccel[i].Normalize();
		pointLightSpeeds[i] += pointLightAccel[i] * deltaTime;
		pointLightSpeeds[i].Normalize();
		pointLights[i]->pointLightData.Position = pointLights[i]->pointLightData.Position + 30 * pointLightSpeeds[i] * deltaTime;

		if (pointLights[i]->pointLightData.Position.x > 60)
			pointLights[i]->pointLightData.Position.x -= 120;
		else if (pointLights[i]->pointLightData.Position.x < -60)
			pointLights[i]->pointLightData.Position.x += 120;

		if (pointLights[i]->pointLightData.Position.y > 10)
			pointLights[i]->pointLightData.Position.y -= 10;
		else if (pointLights[i]->pointLightData.Position.y < 0)
			pointLights[i]->pointLightData.Position.y += 10;

		if (pointLights[i]->pointLightData.Position.z > 60)
			pointLights[i]->pointLightData.Position.z -= 120;
		else if (pointLights[i]->pointLightData.Position.z < -60)
			pointLights[i]->pointLightData.Position.z += 120;
	}
	*/

	/*
	for (size_t i = 0; i < 8; i++)
	{
		if ((pointLightLifeTime[i] + deltaTime) < lifeTime) {
			pointLightLifeTime[i] += deltaTime;
			pointLights[i + _randomPointLights]->pointLightData.Position =
				pointLights[i + _randomPointLights]->pointLightData.Position + pointLightDirections[i] * 20.0f * deltaTime;
			//lightData.pointLights[i + _randomPointLights].Diffuse = Vector4(lightData.pointLights[i + _randomPointLights].Diffuse);

		}
		else {
			pointLights[i + _randomPointLights]->pointLightData.Range = 0.0f;
		}
	}
	*/

	{
		Vector3 _spotDir = (ball->GetCenterLocation() + ball->GetMoveDir() * 5.0f)
			- _sl_1->spotLightData.Position;
		_spotDir.Normalize();
		_sl_1->spotLightData.Direction = _spotDir;
	}

	
}

void KatamariDefGame::Render()
{
	// Отрисовка сцены
	renderer->RenderScene(scene);
}

void KatamariDefGame::SpawnCollectibles()
{
	// обычные объекты
	for (int i = 0; i < 0; ++i)
	{
		float rad = 0.3f;
		float x = (rand() % 20) - 10.0f;
		float z = (rand() % 20) - 10.0f;
		collectibles.push_back(new CollectibleObject(renderer->GetDevice(), rad, DirectX::XMFLOAT3(x, rad, z)));
	}
	// загруженные модельки
	for (int i = 0; i < 10; ++i)
	{
		float x = (rand() % 20) - 10.0f;
		float z = (rand() % 20) - 10.0f;
		collectibles.push_back(new CollectibleObject(renderer->GetDevice(), "models\\", DirectX::XMFLOAT3(x, 0.3f, z)));
	}

	for (auto coll : collectibles)
	{
		scene.AddNode(coll);
		coll->camera = renderer->GetMainCamera();
	}
}


void KatamariDefGame::HandleKeyDown(Keys key) {
	if (key == Keys::W)
	{
		ball->PushForward(deltaTime);
	}
	if (key == Keys::S)
	{

	}
	if (key == Keys::A)
	{
		ball->AddTurn(-1.0f, deltaTime);
	}
	if (key == Keys::D)
	{
		ball->AddTurn(1.0f, deltaTime);
	}
	if (key == Keys::Space)
	{
		/*
		pointLightLifeTime[currPointLightBullet] = 0.0f;
		pointLightDirections[currPointLightBullet] = ball->GetMoveDir();
		//pointLightColors[currPointLightBullet] = { (i + 1) % 2 * 1.0f, i / 3 * 1.0f, (i + 2) % 6 / 3 * 1.0f, 1 };

		pointLights[currPointLightBullet + _randomPointLights]->pointLightData.Position = ball->GetCenterLocation();
		
		std::cout << currPointLightBullet + _randomPointLights << " :: " << pointLights[currPointLightBullet + _randomPointLights]->pointLightData.Position.x << "\n";
		
		Vector4 diffuse = pointLights[currPointLightBullet + _randomPointLights]->pointLightData.Diffuse;
		float att = pointLights[currPointLightBullet + _randomPointLights]->pointLightData.Att.y;
		float c = fmax(fmax(diffuse.x, diffuse.y), diffuse.z) / att;
		float range = 256.0f * c;
		pointLights[currPointLightBullet + _randomPointLights]->pointLightData.Range = range;
		currPointLightBullet = (currPointLightBullet + 1) % 8;
		
		std::cout << ball->GetMoveDir().x << ", " << ball->GetMoveDir().z << "\n";
		*/
	}
}


void KatamariDefGame::HandleMouseMove(const InputDevice::MouseMoveEventArgs& args)
{
	ball->AddTurn(args.Offset.x * 0.1, deltaTime);
	renderer->GetMainCamera()->RotatePitch(-deltaTime * args.Offset.y * 0.1);
	renderer->GetMainCamera()->SetReferenceLen(renderer->GetMainCamera()->GetReferenceLen() - args.WheelDelta * 0.01f);
	//std::cout << "\t\t" << args.WheelDelta << "\n";

}