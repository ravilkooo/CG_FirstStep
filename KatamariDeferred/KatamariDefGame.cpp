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
	LightPass* gLightPass;
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
		pointLights.push_back(new PointLight(
			renderer->GetDevice(),
			{ 15.0f * cos(XM_2PI * i / 6.0f), 2.0f, 15.0f * sin(XM_2PI * i / 6.0f) },
			10.0f,
			{ 0.01f, 1.0f, 0.0f },
			{ 0, 0, 0, 1 },
			// { 0, 0, 0, 1 },
			// { 0, 0, 0, 1 }
			{ (i + 1) % 2 * 1.0f, i / 3 * 1.0f, (i + 2) % 6 / 3 * 1.0f, 1 },
			{ (i + 1) % 2 * 1.0f, i / 3 * 1.0f, (i + 2) % 6 / 3 * 1.0f, 1 }
		));
		scene.AddNode(pointLights[i]);
	}

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<float> dis(0.0f, 1.0f);

	for (int i = 0; i < 8; i++)
	{
		Matrix rotationX = Matrix::CreateRotationX(dis(gen) * XM_2PI);
		Matrix rotationY = Matrix::CreateRotationY(dis(gen) * XM_2PI);
		Matrix rotationZ = Matrix::CreateRotationZ(dis(gen) * XM_2PI);
		float _dist = dis(gen) * 2.0f + 5.0f;
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

		Matrix rotation = rotationX * rotationY * rotationZ;

		pointLights.push_back(new PointLight(
			renderer->GetDevice(),
			{ 0.0f, 0.0f, 0.0f },
			0.0f,
			{ 0.01f, 0.1f, 0.0f },
			{ 0, 0, 0, 1 },
			{ lightColor.x, lightColor.y, lightColor.z, 1.0f },
			{ lightColor.x, lightColor.y, lightColor.z, 1.0f }
		));

		//pointLightPositions[i] = Vector3::Transform({ _dist, 0.0f, 0.0f }, rotation);
		pointLightLifeTime[i] = lifeTime * 2.0f;

		scene.AddNode(pointLights[i + 6]);
	}

	_sl_1 = new SpotLight(renderer->GetDevice(), { 0.0f, 15.0f, 0.0f }, 5.0f, { 0.0f, 1.0f, 0.0f },
		10, { 0.01f, 0.05f, 0.0f },
		{ 0, 0.1, 0, 1 }, { 1, 1, 1, 1 }, { 1, 1, 1, 1 });
	scene.AddNode(_sl_1);

	FullScreenQuad* fsq = new FullScreenQuad(renderer->GetDevice());
	scene.AddNode(fsq);

	for (SceneNode* node : scene.nodes) {
		node->camera = renderer->GetMainCamera();
	}


}

KatamariDefGame::~KatamariDefGame()
{
}

void KatamariDefGame::Update(float deltaTime)
{

	//std::cout << floor->binds.size() << " " << ball->binds.size() << " " << "\n";
	// delete this

	ball->SlowDown(deltaTime);
	physEngine->Update(deltaTime);


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



	for (size_t i = 0; i < 6; i++)
	{
		pointLights[i]->pointLightData.Position =
			Vector3::Transform(pointLights[i]->pointLightData.Position,
				Matrix::CreateRotationY(deltaTime * 1.0f));
	}
	for (size_t i = 0; i < 8; i++)
	{
		if ((pointLightLifeTime[i] + deltaTime) < lifeTime) {
			pointLightLifeTime[i] += deltaTime;
			pointLights[i + 6]->pointLightData.Position =
				pointLights[i + 6]->pointLightData.Position + pointLightDirections[i] * 20.0f * deltaTime;
			//lightData.pointLights[i + 6].Diffuse = Vector4(lightData.pointLights[i + 6].Diffuse);
		}
		else {
			pointLights[i + 6]->pointLightData.Range = 0.0f;
		}
	}
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
		pointLightLifeTime[currPointLightBullet] = 0.0f;
		pointLightDirections[currPointLightBullet] = ball->GetMoveDir();
		//pointLightColors[currPointLightBullet] = { (i + 1) % 2 * 1.0f, i / 3 * 1.0f, (i + 2) % 6 / 3 * 1.0f, 1 };

		pointLights[currPointLightBullet + 6]->pointLightData.Position = ball->GetCenterLocation();
		Vector4 diffuse = pointLights[currPointLightBullet + 6]->pointLightData.Diffuse;
		float att = pointLights[currPointLightBullet + 6]->pointLightData.Att.y;
		float c = fmax(fmax(diffuse.x, diffuse.y), diffuse.z) / att;
		float range = (16.0f * sqrtf(c) + 1.0f);
		pointLights[currPointLightBullet + 6]->pointLightData.Range = range;
		currPointLightBullet = (currPointLightBullet + 1) % 8;
		//std::cout << ball->GetMoveDir().x << ", " << ball->GetMoveDir().z << "\n";
	}
}


void KatamariDefGame::HandleMouseMove(const InputDevice::MouseMoveEventArgs& args)
{
	ball->AddTurn(args.Offset.x * 0.1, deltaTime);
	renderer->GetMainCamera()->RotatePitch(-deltaTime * args.Offset.y * 0.1);
	renderer->GetMainCamera()->SetReferenceLen(renderer->GetMainCamera()->GetReferenceLen() - args.WheelDelta * 0.01f);
	//std::cout << "\t\t" << args.WheelDelta << "\n";

}