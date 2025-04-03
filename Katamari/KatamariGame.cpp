#include "KatamariGame.h"

// временно
#include "DDSTextureLoader.h"
#include <StringHelper.h>

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

	renderer = Renderer(&displayWindow);
	renderer.camera = Camera(winWidth * 1.0f / winHeight);

	ball = new StickyBall(renderer.GetDevice());
	ball->camera = &(renderer.camera);

	floor = new Floor(renderer.GetDevice());
	floor->camera = &(renderer.camera);
	scene.AddNode(ball);
	scene.AddNode(floor);

	SpawnCollectibles();

	renderer.camera.SwitchToFollowMode(ball->position, ball->GetMoveDir(), ball->radius);

	// Light

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

	light_pcb = new Bind::PixelConstantBuffer<LightData>(renderer.GetDevice(), lightData, 0u);
	renderer.AddPerFrameBind(light_pcb);

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
	renderer.camera.Update(deltaTime, ball->worldMat, ball->GetMoveDir(), ball->radius);

	Matrix vpMat = renderer.camera.GetViewMatrix() * renderer.camera.GetProjectionMatrix();


	XMFLOAT3 camera_pos = renderer.camera.GetPosition();
	for (auto coll : collectibles)
	{
		coll->vcb->Update(renderer.GetDeviceContext(),
			{
				coll->isAttached ? ball->position : coll->initialPosition
			}
		);
		coll->pcb->Update(renderer.GetDeviceContext(),
			{
				camera_pos
			});
	}

	ball->pcb->Update(renderer.GetDeviceContext(),
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
	

	light_pcb->Update(renderer.GetDeviceContext(),
		{
			lightData
		}
	);




	floor->pcb->Update(renderer.GetDeviceContext(),
		{
			camera_pos
		});

}

void KatamariGame::Render()
{
	// Отрисовка сцены
	renderer.RenderScene(scene);
}

void KatamariGame::SpawnCollectibles()
{
	// обычные объекты
	for (int i = 0; i < 0; ++i)
	{
		float rad = 0.3f;
		float x = (rand() % 20) - 10.0f;
		float z = (rand() % 20) - 10.0f;
		collectibles.push_back(new CollectibleObject(renderer.GetDevice(), rad, DirectX::XMFLOAT3(x, rad, z)));
	}
	// загруженные модельки
	for (int i = 0; i < 10; ++i)
	{
		float x = (rand() % 20) - 10.0f;
		float z = (rand() % 20) - 10.0f;
		collectibles.push_back(new CollectibleObject(renderer.GetDevice(), "models\\", DirectX::XMFLOAT3(x, 0.3f, z)));
	}

	for (auto coll : collectibles)
	{
		scene.AddNode(coll);
		coll->camera = &(renderer.camera);
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
		std::cout << ball->GetMoveDir().x << ", " << ball->GetMoveDir().z << "\n";
	}
}


void KatamariGame::HandleMouseMove(const InputDevice::MouseMoveEventArgs& args)
{
	ball->AddTurn(args.Offset.x * 0.1, deltaTime);
	renderer.camera.RotatePitch(-deltaTime * args.Offset.y * 0.1);
}