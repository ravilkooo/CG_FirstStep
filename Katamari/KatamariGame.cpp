#include "KatamariGame.h"

KatamariGame::KatamariGame()
{
	Initialize();
}

KatamariGame::~KatamariGame()
{
}

void KatamariGame::Initialize()
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

	scene.AddNode(&ball);
	scene.AddNode(&floor);

	physEngine = new PhysicsEngine(&scene);

	displayWindow = DisplayWindow(applicationName, hInstance, winWidth, winHeight);
	inputHandler = displayWindow.GetInputHandler();

	renderer = Renderer(&displayWindow);
	renderer.camera = Camera(winWidth * 1.0f / winHeight);

	for (auto node : scene.nodes)
	{
		node->LoadAndCompileShader(renderer.shaderManager);
		node->InitBuffers(renderer.resourceManager);
		node->camera = &(renderer.camera);
		//std::cout << "f1\n";
	}
	SpawnCollectibles();

	renderer.camera.SwitchToFollowMode(ball.position, ball.GetMoveDir(), ball.radius);
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
		float deltaTime = timer.GetDeltaTime();
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
	// Получаем InputHandler из DisplayWindow
	inputHandler = displayWindow.GetInputHandler();

	if (inputHandler->IsKeyDown(InputHandler::KeyCode::W))
	{
		ball.PushForward(deltaTime);
	}
	else if (inputHandler->IsKeyDown(InputHandler::KeyCode::S))
	{
		
	}
	else if (inputHandler->IsKeyDown(InputHandler::KeyCode::A))
	{
		ball.AddTurn(-1.0f, deltaTime);
	}
	else if (inputHandler->IsKeyDown(InputHandler::KeyCode::D))
	{
		ball.AddTurn(1.0f, deltaTime);
	}
	else {
	}
	ball.SlowDown(deltaTime);
	physEngine->Update(deltaTime);

	renderer.camera.Update(deltaTime, ball.worldMat, ball.GetMoveDir(), ball.radius);

	// Проверка коллизий
	for (auto& obj : collectibles)
	{
		if (obj.CheckCollision(ball))
		{
			obj.AttachToBall(&ball);
			ball.Grow(obj.radius / deltaTime);
		}
	}

}

void KatamariGame::Render()
{
	// Отрисовка сцены
	renderer.RenderScene(scene);
}

void KatamariGame::SpawnCollectibles()
{
	// Генерация объектов
	for (int i = 0; i < 10; ++i)
	{
		float x = (rand() % 20) - 10.0f;
		float z = (rand() % 20) - 10.0f;
		collectibles.emplace_back(0.3f, DirectX::XMFLOAT3(x, 0.3f, z));
	}

	for (auto& obj : collectibles)
	{
		scene.AddNode(&obj);
		obj.LoadAndCompileShader(renderer.shaderManager);
		obj.InitBuffers(renderer.resourceManager);
		obj.camera = &(renderer.camera);
	}
}