#include "GameQuads.h"


GameQuads::GameQuads()
{
    applicationName = L"SunShine_0";
    hInstance = GetModuleHandle(nullptr);
    //inputHandler = InputHandler();

    timer = GameTimer();

    scene = Scene();

    scene.AddNode(new Quad());
    scene.AddNode(new Quad(DirectX::XMFLOAT4(-1., -1., 0, 1), 0.3));
    scene.AddNode(new Quad(DirectX::XMFLOAT4(0., 0.5, 0, 1), 1.2, 0.4));
    scene.AddNode(new Triangle());

    physEngine =  new PhysicsEngine(&scene);

    displayWindow = DisplayWindow(this, applicationName, hInstance, winWidth, winHeight);


    renderer = Renderer(&displayWindow);


    for (auto node : scene.nodes)
    {
        node->LoadAndCompileShader(renderer.shaderManager);
        node->InitBuffers(renderer.resourceManager);
        std::cout << "f1\n";
    }
}


void GameQuads::Run()
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

void GameQuads::Update(float deltaTime)
{
    physEngine->Update(deltaTime);
    // Обновление состояния игры
}

void GameQuads::Render()
{
    // Отрисовка сцены
    renderer.RenderScene(scene);
}



GameQuads::~GameQuads()
{
}