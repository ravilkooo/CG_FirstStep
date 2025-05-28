#include "KatamariGame.h"

KatamariGame::KatamariGame()
{
	applicationName = L"KatamariGame";
	hInstance = GetModuleHandle(nullptr);

	winWidth = 1000;
	winHeight = 800;

	timer = GameTimer();

	scene = Scene();

	physEngine = new PhysicsEngine(&scene);

	displayWindow = DisplayWindow(this, applicationName, hInstance, winWidth, winHeight);

	renderer = new ForwardRenderer(&displayWindow);

	floor = new Floor(renderer->GetDevice());
	ball = new StickyBall(renderer->GetDevice());

	scene.AddNode(floor);
	scene.AddNode(ball);

	SpawnCollectibles();


	// Light
	Vector3 lightDir = { 0, -30, 80 };
	lightDir.Normalize();

	lightData.dLight = {
		Vector4(0.1f, 0.1f, 0.1f, 0.0f),
		Vector4(0.4f, 0.4f, 0.4f, 0.0f),
		Vector4(0.6f, 0.6f, 0.6f, 0.0f),
		lightDir,
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
		//std::cout << (i + 1) % 2 * 1.0f << ", " << i / 3 * 1.0f << ", " << (i + 2) % 6 / 3 * 1.0f << "\n";
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

		lightData.pointLights[i + 6].Ambient = { 0, 0, 0, 1 };
		lightData.pointLights[i + 6].Diffuse = { lightColor.x, lightColor.y, lightColor.z, 1.0f };
		lightData.pointLights[i + 6].Specular = { lightColor.x, lightColor.y, lightColor.z, 1.0f };

		//pointLightPositions[i] = Vector3::Transform({ _dist, 0.0f, 0.0f }, rotation);
		pointLightLifeTime[i] = lifeTime * 2.0f;

		lightData.pointLights[i + 6].Range = 0.0f;
		lightData.pointLights[i + 6].Att = { 0.01f, 0.1f, 0.0f };
	}

	lightData.spotLight = {
		Vector4(0.1f, 0.1f, 0.1f, 0.0f),
		Vector4(0.7f, 0.7f, 0.7f, 0.7f),
		Vector4(0.9f, 0.9f, 0.9f, 0.0f),
		Vector3(0.0f, 15.0f, 0.0f),
		100.0f,
		Vector3(0.0f, 1.0f, 0.0f),
		16.0f,
		{ 0.01f, 0.02f, 0.0f },
		0
	};

	Vector3 _spotDir = ball->position + ball->GetMoveDir() * 5.0f - Vector3(0.0f, 15.0f, 0.0f);
	_spotDir.Normalize();
	lightData.spotLight.Direction = _spotDir;

	UINT smSizeX = 1024;
	UINT smSizeY = 1024;

	// DL_ShadowMapPass
	dl_shadowMapPass = new DL_ShadowMapPass(renderer->GetDevice(),
		renderer->GetDeviceContext(), smSizeX, smSizeY, lightData.dLight);
	renderer->AddPass(dl_shadowMapPass);

	// MainColorPass
	{
		MainColorPass* colorPass = new MainColorPass(renderer->GetDevice(), renderer->GetDeviceContext(),
			renderer->GetBackBuffer(), winWidth, winHeight);

		renderer->SetMainCamera(colorPass->GetCamera());

		colorPass->camera->SwitchToFollowMode(ball->position, ball->GetMoveDir(), ball->radius);


		D3D11_DEPTH_STENCIL_DESC depthStencilDesc = {};
		depthStencilDesc.DepthEnable = true;
		depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

		colorPass->AddPerFrameBind(new Bind::DepthStencilState(renderer->GetDevice(), depthStencilDesc));

		// --- Light ---
		light_pcb = new Bind::PixelConstantBuffer<LightData_old>(renderer->GetDevice(), lightData, 0u);
		colorPass->AddPerFrameBind(light_pcb);

		// --- Shadow stuff ---
		
		// View texture as Shader resource while using it for shadowing in pixel shader
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
		srvDesc.Texture2DArray.MostDetailedMip = 0;
		srvDesc.Texture2DArray.MipLevels = 1;
		srvDesc.Texture2DArray.FirstArraySlice = 0;
		srvDesc.Texture2DArray.ArraySize = 4;

		colorPass->AddPerFrameBind(new Bind::TextureB(renderer->GetDevice(),
			dl_shadowMapPass->GetTexture(), srvDesc, 0u));
		

		// Sampler of texture. It samples values from texture
		D3D11_SAMPLER_DESC shadowSamplerDesc;
		shadowSamplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
		shadowSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
		shadowSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
		shadowSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
		shadowSamplerDesc.MipLODBias = 0.0f;
		shadowSamplerDesc.MaxAnisotropy = 1;
		shadowSamplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		shadowSamplerDesc.BorderColor[0] = 1.0;
		shadowSamplerDesc.BorderColor[1] = 1.0;
		shadowSamplerDesc.BorderColor[2] = 1.0;
		shadowSamplerDesc.BorderColor[3] = 1.0;
		shadowSamplerDesc.MinLOD = 0;
		shadowSamplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

		colorPass->AddPerFrameBind(new Bind::Sampler(renderer->GetDevice(), shadowSamplerDesc, 0u));

		shadowSamplerDesc.Filter = D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
		shadowSamplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_BORDER;
		shadowSamplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_BORDER;
		shadowSamplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_BORDER;
		shadowSamplerDesc.MipLODBias = 0.0f;
		shadowSamplerDesc.MaxAnisotropy = 1;
		shadowSamplerDesc.ComparisonFunc = D3D11_COMPARISON_LESS;
		shadowSamplerDesc.BorderColor[0] = 1.0;
		shadowSamplerDesc.BorderColor[1] = 1.0;
		shadowSamplerDesc.BorderColor[2] = 1.0;
		shadowSamplerDesc.BorderColor[3] = 1.0;
		shadowSamplerDesc.MinLOD = 0;
		shadowSamplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

		colorPass->AddPerFrameBind(new Bind::Sampler(renderer->GetDevice(), shadowSamplerDesc, 1u));

		// All Cascades (for pixel shader while drawing final scene)
		colorPass->AddPerFrameBind(dl_shadowMapPass->cascadesConstantBuffer);


		// Textures for objects (skin)
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
		
		colorPass->AddPerFrameBind(new Bind::Sampler(renderer->GetDevice(), samplerDesc, 2u));

		colorPass->AddPerFrameBind(new Bind::TextureB(renderer->GetDevice(), "PawBgBlue.dds", aiTextureType_DIFFUSE, 2u));

		samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_MIRROR;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_MIRROR;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_MIRROR;
		samplerDesc.MipLODBias = 0.0f;
		samplerDesc.MaxAnisotropy = 1;
		samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
		samplerDesc.BorderColor[0] = 0;
		samplerDesc.BorderColor[1] = 0;
		samplerDesc.BorderColor[2] = 0;
		samplerDesc.BorderColor[3] = 0;
		samplerDesc.MinLOD = 0;
		samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

		colorPass->AddPerFrameBind(new Bind::Sampler(renderer->GetDevice(), samplerDesc, 3u));

		renderer->AddPass(colorPass);
	}

	for (size_t i = 0; i < 4; i++)
	{
		Camera fCam = Camera(*(dl_shadowMapPass->GetFrustumCamera()));

		float nearZ; float farZ;
		dl_shadowMapPass->GetFrustumBoundsZ(i, &nearZ, &farZ);
		fCam.SetNearZ(nearZ);
		fCam.SetFarZ(farZ);
		FrustrumWireframe* fwf = new FrustrumWireframe(renderer->GetDevice(), fCam);
		scene.AddNode(fwf);
	}
	

	for (SceneNode* node : scene.nodes) {
		node->camera = renderer->GetMainCamera();
	}

	InputDevice::getInstance().OnKeyPressed.AddRaw(this, &KatamariGame::HandleKeyDown);
	InputDevice::getInstance().MouseMove.AddRaw(this, &KatamariGame::HandleMouseMove);

}

KatamariGame::~KatamariGame()
{
}

void KatamariGame::Run()
{
	MSG msg = {};
	bool isExitRequested = false;
	unsigned int frameCount = 0;
	float totalTime = 0;

	while (!isExitRequested) {
		// Handle the windows messages.
		while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		// If windows signals to end the application then exit out.
		if (msg.message == WM_QUIT) {
			isExitRequested = true;
		}
		timer.Tick();
		deltaTime = timer.GetDeltaTime();
		totalTime += deltaTime;
		frameCount++;

		if (totalTime > 1.0f) {
			float fps = frameCount * 1.0f / totalTime;

			totalTime -= 1.0f;

			WCHAR text[256];
			swprintf_s(text, TEXT("FPS: %f"), fps);
			SetWindowText(displayWindow.hWnd, text);

			frameCount = 0;
		}

		Update(deltaTime);
		Render();
	}
}

void KatamariGame::Update(float deltaTime)
{
	//std::cout << ball->GetCenterLocation().x << ",\t" << ball->GetCenterLocation().y << ",\t" << ball->GetCenterLocation().z << "\n";
	//std::cout << floor->binds.size() << " " << ball->binds.size() << " " << "\n";
	// delete this
	dl_shadowMapPass->SetPlayerCamera(renderer->GetMainCamera());

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
	renderer->GetMainCamera()->Update(deltaTime, ball->worldMat, ball->GetMoveDir(), ball->radius);

	//Matrix vpMat = renderer->camera.GetViewMatrix() * renderer->camera.GetProjectionMatrix();


	XMFLOAT3 camera_pos = renderer->GetMainCamera()->GetPosition();
	for (auto coll : collectibles)
	{
		coll->vcb->Update(renderer->GetDeviceContext(),
			{
				coll->isAttached ? ball->position : coll->initialPosition
			}
		);
		coll->pcb->Update(renderer->GetDeviceContext(),
			{
				camera_pos
			});
	}

	ball->pcb->Update(renderer->GetDeviceContext(),
		{
			camera_pos
		});



	for (size_t i = 0; i < 6; i++)
	{
		lightData.pointLights[i].Position = Vector3::Transform(lightData.pointLights[i].Position, Matrix::CreateRotationY(deltaTime * 1.0f));
	}
	for (size_t i = 0; i < 8; i++)
	{
		if ((pointLightLifeTime[i] + deltaTime) < lifeTime) {
			pointLightLifeTime[i] += deltaTime;
			lightData.pointLights[i + 6].Position = lightData.pointLights[i + 6].Position + pointLightDirections[i] * 20.0f * deltaTime;
			//lightData.pointLights[i + 6].Diffuse = Vector4(lightData.pointLights[i + 6].Diffuse);
		}
		else {
			lightData.pointLights[i + 6].Range = 0.0f;
		}
	}
	{
		Vector3 _spotDir = (ball->GetCenterLocation() + ball->GetMoveDir() * 5.0f) - lightData.spotLight.Position;
		_spotDir.Normalize();
		lightData.spotLight.Direction = _spotDir;
	}
	

	light_pcb->Update(renderer->GetDeviceContext(),
		{
			lightData
		}
	);




	floor->pcb->Update(renderer->GetDeviceContext(),
		{
			renderer->GetMainCamera()->GetViewMatrix(),
			camera_pos
		});

}

void KatamariGame::Render()
{
	// Отрисовка сцены
	renderer->RenderScene(scene);
}

void KatamariGame::SpawnCollectibles()
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


void KatamariGame::HandleKeyDown(Keys key) {
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
		lightData.pointLights[currPointLightBullet + 6].Position = ball->GetCenterLocation();
		lightData.pointLights[currPointLightBullet + 6].Range = 10.0f;
		currPointLightBullet = (currPointLightBullet + 1) % 8;
	}
}


void KatamariGame::HandleMouseMove(const InputDevice::MouseMoveEventArgs& args)
{
	ball->AddTurn(args.Offset.x * 0.1, deltaTime);
	renderer->GetMainCamera()->RotatePitch(-deltaTime * args.Offset.y * 0.1);
}