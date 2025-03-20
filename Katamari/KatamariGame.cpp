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

	// Добавление объектов на сцену
	/*for (auto body : cosmicBodies)
	{
		scene.AddNode(body);
	}*/

	physEngine = new PhysicsEngine(&scene);

	displayWindow = DisplayWindow(this, applicationName, hInstance, winWidth, winHeight);
	// inputHandler = displayWindow.GetInputHandler();

	renderer = Renderer(&displayWindow);
	renderer.camera = Camera(winWidth * 1.0f / winHeight);

	ball = StickyBall(renderer.GetDevice());
	floor = Floor(renderer.GetDevice());
	scene.AddNode(&ball);
	scene.AddNode(&floor);

	SpawnCollectibles();

	for (auto node : scene.nodes)
	{
		node->device = renderer.GetDevice();
		node->LoadAndCompileShader(renderer.shaderManager);
		node->InitBuffers(renderer.resourceManager);

		if (node->hasTexture) {
			//std::cout << "hasTexture\n";


			// node->textures.push_back(Texture(renderer.GetDevice(), SE_Colors::UnloadedTextureColor, aiTextureType_DIFFUSE));
			/*
			node->texture = new Texture(renderer.GetDevice());

			HRESULT hr = CreateDDSTextureFromFile( renderer.GetDevice(),
				StringHelper::StringToWide("models\\Textures\\plane_Diffuse.dds").c_str(),
				&(node->texture->pTexture),
				&(node->texture->pTextureView));
			if (FAILED(hr))
			{
				std::cout << "FAILED TEXTURE LOAD: " << "models\\Textures\\plane_Diffuse.dds" << "\n";
			}

			node->texture->Bind(renderer.GetDeviceContext());
			node->textureSampler = new Sampler(renderer.GetDevice());
			node->textureSampler->Bind(renderer.GetDeviceContext());
			*/
		}

		node->camera = &(renderer.camera);
		//std::cout << "f1\n";
	}

	renderer.camera.SwitchToFollowMode(ball.position, ball.GetMoveDir(), ball.radius);

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

			//std::cout << frameCount << "\n";

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
	ball.SlowDown(deltaTime);
	physEngine->Update(deltaTime);


	// Проверка коллизий
	for (auto& obj : collectibles)
	{
		if (obj.CheckCollision(ball))
		{
			obj.AttachToBall(&ball);
			ball.Grow(obj.radius / deltaTime);
		}
	}
	renderer.camera.Update(deltaTime, ball.worldMat, ball.GetMoveDir(), ball.radius);

	Matrix vpMat = renderer.camera.GetViewMatrix() * renderer.camera.GetProjectionMatrix();

	for (auto node : scene.nodes)
	{
		node->cb.wvpMat = node->worldMat * (XMMATRIX)vpMat;
	}

}

void KatamariGame::Render()
{
	// Отрисовка сцены
	renderer.RenderScene(scene);
}

void KatamariGame::SpawnCollectibles()
{
	for (int i = 0; i < 3; ++i)
	{
		float rad = 0.3f;
		float x = (rand() % 20) - 10.0f;
		float z = (rand() % 20) - 10.0f;
		collectibles.emplace_back(renderer.GetDevice(), rad, DirectX::XMFLOAT3(x, rad, z));
	}
	// Генерация объектов
	for (int i = 0; i < 10; ++i)
	{
		float x = (rand() % 20) - 10.0f;
		float z = (rand() % 20) - 10.0f;
		//collectibles.emplace_back(0.3f, DirectX::XMFLOAT3(x, 0.3f, z));
		collectibles.emplace_back(renderer.GetDevice(), "models\\", DirectX::XMFLOAT3(x, 0.3f, z));
	}

	for (auto& obj : collectibles)
	{
		scene.AddNode(&obj);
		obj.LoadAndCompileShader(renderer.shaderManager);
		obj.InitBuffers(renderer.resourceManager);
		obj.camera = &(renderer.camera);
	}
}


void KatamariGame::HandleKeyDown(Keys key) {
	if (key == Keys::W)
	{
		ball.PushForward(deltaTime);
	}
	if (key == Keys::S)
	{

	}
	if (key == Keys::A)
	{
		ball.AddTurn(-1.0f, deltaTime);
	}
	if (key == Keys::D)
	{
		ball.AddTurn(1.0f, deltaTime);
	}
}


void KatamariGame::HandleMouseMove(const InputDevice::MouseMoveEventArgs& args)
{
	ball.AddTurn(args.Offset.x * 0.1, deltaTime);
	//renderer.camera.RotatePitch(-deltaTime * args.Offset.y * 0.1);
}