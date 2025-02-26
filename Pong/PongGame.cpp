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

    border = new Border();

    scene = Scene();
    scene.AddNode(border);
    scene.AddNode(new Gates(DirectX::XMFLOAT4(-0.8, 0.5, 0.25, 1), 0.1));
    scene.AddNode(new Gates(DirectX::XMFLOAT4(0.8, 0.5, 0.25, 1), 0.1));

    ball = new Ball(DirectX::XMFLOAT4(0., 0., 0.25, 1), 0.1);
    scene.AddNode(ball);
    scene.AddNode(new Racket(DirectX::XMFLOAT4(-0.8, 0.5, 0.25, 1), 0.1, 0.4, DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f)));
    scene.AddNode(new Racket(DirectX::XMFLOAT4(0.8, 0.5, 0.25, 1), 0.1, 0.4, DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f)));


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
    if (CheckBorderCollision())
    {

        std::cout << "HH\n";
        border->HitBall(ball);
    }
        
}

void PongGame::Render()
{
    // Отрисовка сцены
    renderer.RenderScene(scene);
}

bool PongGame::CheckBorderCollision()
{

    DirectX::BoundingBox* racketBox = border->GetBoundingBoxes();
    DirectX::BoundingBox ballBox = ball->GetBoundingBox();
    std::cout << racketBox[0].Center.x << " " << racketBox[1].Center.x << ballBox.Center.x << "\n";
    std::cout << racketBox[0].Center.y << " " << racketBox[1].Center.y << ballBox.Center.y << "\n";
    std::cout << racketBox[0].Center.z << " " << racketBox[1].Center.z << ballBox.Center.z << "\n";
    return (racketBox[0].Intersects(ballBox) || racketBox[1].Intersects(ballBox));
}