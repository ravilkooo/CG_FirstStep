#include "PongGame.h"


PongGame::PongGame()
{
    Initialize();
}

PongGame::~PongGame()
{
}

void PongGame::Run()
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
            float fps = frameCount * 1. / totalTime;

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

void PongGame::Initialize()
{

    applicationName = L"SunShine_0";
    hInstance = GetModuleHandle(nullptr);
    //inputHandler = InputHandler();

    timer = GameTimer();

    scene = Scene();
    scene.AddNode(new Border());
    scene.AddNode(new Gates(DirectX::XMFLOAT4(-0.8, 0.5, 0, 1), 0.1));
    scene.AddNode(new Gates(DirectX::XMFLOAT4(0.8, 0.5, 0, 1), 0.1));

    scene.AddNode(new Ball(DirectX::XMFLOAT4(0., 0., 0, 1), 0.1));
    scene.AddNode(new Racket(DirectX::XMFLOAT4(-0.8, 0.5, 0, 1), 0.1, 0.4, DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f)));
    scene.AddNode(new Racket(DirectX::XMFLOAT4(0.8, 0.5, 0, 1), 0.1, 0.4, DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f)));


    physEngine = PhysicsEngine(&scene);

    displayWindow = DisplayWindow(applicationName, hInstance, 800, 800);


    renderer = Renderer(&displayWindow);


    for (auto node : scene.nodes)
    {
        node->LoadAndCompileShader(renderer.shaderManager);
        node->InitBuffers(renderer.resourceManager);
        std::cout << "f1\n";
    }
}

void PongGame::Update(float deltaTime)
{
    physEngine.Update(deltaTime);
    // Обновление состояния игры
}

void PongGame::Render()
{
    // Отрисовка сцены
    renderer.RenderScene(scene);
}