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

	ball = new StickyBall(renderer.GetDevice());
	floor = new Floor(renderer.GetDevice());
	scene.AddNode(ball);
	scene.AddNode(floor);

	SpawnCollectibles();

	for (auto node : scene.nodes)
	{
		node->device = renderer.GetDevice();

		// TO-DO: Change to SceneNode.AddBind()
		node->LoadAndCompileShader(renderer.shaderManager);

		// TO-DO: Change to SceneNode.AddBind()
		node->InitBuffers(renderer.resourceManager);

		node->camera = &(renderer.camera);
		//std::cout << "f1\n";
	}
	ball->pcb = new Bind::PixelConstantBuffer<StickyBall::Ball_PCB>(renderer.GetDevice(), ball->ball_pcb);
	ball->AddBind(ball->pcb);

	ball->vcb = new Bind::VertexConstantBuffer<StickyBall::Ball_VCB>(renderer.GetDevice(), ball->ball_vcb);
	ball->AddBind(ball->vcb);

	for (auto& obj : collectibles)
	{
		obj.pcb = new Bind::PixelConstantBuffer<CollectibleObject::Collectible_PCB>(renderer.GetDevice(), obj.coll_pcb);
		obj.AddBind(obj.pcb);

		obj.vcb = new Bind::VertexConstantBuffer<CollectibleObject::Collectible_VCB>(renderer.GetDevice(), obj.coll_vcb);
		obj.AddBind(obj.vcb);
	}
	//std::cout << floor->binds.size() << " " << ball->binds.size() << " " << "\n";

	renderer.camera.SwitchToFollowMode(ball->position, ball->GetMoveDir(), ball->radius);

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
	for (auto& obj : collectibles)
	{
		if (obj.CheckCollision(ball))
		{
			obj.AttachToBall(ball);
			ball->Grow(obj.radius / deltaTime);
		}
	}
	renderer.camera.Update(deltaTime, ball->worldMat, ball->GetMoveDir(), ball->radius);

	Matrix vpMat = renderer.camera.GetViewMatrix() * renderer.camera.GetProjectionMatrix();


	ball->pcb->Update(renderer.GetDeviceContext(), ball->ball_pcb);
	ball->vcb->Update(renderer.GetDeviceContext(), {
			ball->worldMat,
			vpMat,
			// ball->radius
		}
		);

	for (auto& obj : collectibles)
	{
		obj.pcb->Update(renderer.GetDeviceContext(), obj.coll_pcb);
		obj.vcb->Update(renderer.GetDeviceContext(),
			{
				obj.worldMat,
				vpMat,
				obj.isAttached ? ball->position : obj.initialPosition
			}
		);
	}
	floor->cb.wvpMat = floor->worldMat * (XMMATRIX)vpMat;


	/*for (auto node : scene.nodes)
	{
		node->cb.wvpMat = node->worldMat * (XMMATRIX)vpMat;
	}*/

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
		collectibles.emplace_back(renderer.GetDevice(), rad, DirectX::XMFLOAT3(x, rad, z));
	}
	// загруженные модельки
	for (int i = 0; i < 10; ++i)
	{
		float x = (rand() % 20) - 10.0f;
		float z = (rand() % 20) - 10.0f;
		collectibles.emplace_back(renderer.GetDevice(), "models\\", DirectX::XMFLOAT3(x, 0.3f, z));
	}

	for (auto& obj : collectibles)
	{
		scene.AddNode(&obj);
		
		// TO-DO: Change to SceneNode.AddBind()
		obj.LoadAndCompileShader(renderer.shaderManager);

		// TO-DO: Change to SceneNode.AddBind()
		obj.InitBuffers(renderer.resourceManager);

		obj.camera = &(renderer.camera);
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
}


void KatamariGame::HandleMouseMove(const InputDevice::MouseMoveEventArgs& args)
{
	ball->AddTurn(args.Offset.x * 0.1, deltaTime);
	renderer.camera.RotatePitch(-deltaTime * args.Offset.y * 0.1);
}