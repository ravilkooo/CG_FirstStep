#include "GravitationGame.h"



GravitationGame::GravitationGame()
{
	applicationName = L"SolarSystem";
	hInstance = GetModuleHandle(nullptr);

	timer = GameTimer();

	scene = Scene();

	float accum_dist = 0.0f;
	float planet_gap = 0.1f;
	float moon_gap = 0.01f;

	float sun_rad = 0.3f;
	float merc_rad = 0.01f;
	float venus_rad = 0.04f;

	float earth_rad = 0.07f;
	float moon_rad = 0.01f;

	float mars_rad = 0.05f;
	float fobos_rad = 0.007f;
	float deimos_rad = 0.005f;

	float jupiter_rad = 0.1f;

	float saturn_rad = 0.15f;
	float saturn_ring_rad = 0.22f;

	float uranus_rad = 0.09f;
	float uranus_ring_rad = 0.11f;


	// Создание планет и лун
	accum_dist = sun_rad;
	GravitationBody* sun = new GravitationBody(sun_rad, 0.2f, DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f), DirectX::XMFLOAT4(1.0f, 1.0f, 0.0f, 1.0f), GravitationBody::PLANET_TYPE::SPHERE, 100.0f);
	gravBodies.push_back(sun);

	accum_dist += planet_gap + merc_rad;
	GravitationBody* mercury = new GravitationBody(merc_rad, 1.9f, DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f), DirectX::XMFLOAT4(0.6f, 0.0f, 0.0f, 1.0f), GravitationBody::PLANET_TYPE::GEOSPHERE);
	gravBodies.push_back(mercury);
	gravBodies.back()->position = Vector3::Transform(Vector3(0.0f, 0.0f, accum_dist), GetRandomRotateTransform());
	gravBodies.back()->velocity = Vector3::Transform(Vector3(0.0f, 0.0f, GravitationBody::maxVelocity * 0.5), GetRandomRotateTransform());


	accum_dist += merc_rad + planet_gap + venus_rad;
	GravitationBody* venus = new GravitationBody(venus_rad, 0.8f, DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f), DirectX::XMFLOAT4(1.0f, 0.5f, 0.5f, 1.0f), GravitationBody::PLANET_TYPE::CUBE);
	gravBodies.push_back(venus);
	gravBodies.back()->position = Vector3::Transform(Vector3(0.0f, 0.0f, accum_dist), GetRandomRotateTransform());
	gravBodies.back()->velocity = Vector3::Transform(Vector3(0.0f, 0.0f, GravitationBody::maxVelocity * 0.5), GetRandomRotateTransform());

	accum_dist += venus_rad + planet_gap + (2 * moon_rad + moon_gap + earth_rad);
	GravitationBody* earth = new GravitationBody(earth_rad, 0.8f, DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f), DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f), GravitationBody::PLANET_TYPE::SPHERE);
	gravBodies.push_back(earth);
	gravBodies.back()->position = Vector3::Transform(Vector3(0.0f, 0.0f, accum_dist), GetRandomRotateTransform());
	gravBodies.back()->velocity = Vector3::Transform(Vector3(0.0f, 0.0f, GravitationBody::maxVelocity * 0.5), GetRandomRotateTransform());

	accum_dist += (2 * moon_rad + moon_gap + earth_rad) + planet_gap + (2 * fobos_rad + 2 * deimos_rad + 2 * moon_gap + mars_rad);

	//std::cout << mars_rad + moon_gap + fobos_rad << "\n";

	GravitationBody* mars = new GravitationBody(mars_rad, 0.8f, DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f), DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f), GravitationBody::PLANET_TYPE::SPHERE);
	gravBodies.push_back(mars);
	gravBodies.back()->position = Vector3::Transform(Vector3(0.0f, 0.0f, accum_dist), GetRandomRotateTransform());
	gravBodies.back()->velocity = Vector3::Transform(Vector3(0.0f, 0.0f, GravitationBody::maxVelocity * 0.5), GetRandomRotateTransform());

	accum_dist += (2 * fobos_rad + 2 * deimos_rad + 2 * moon_gap + mars_rad) + moon_gap + merc_rad;
	
	gravBodies.push_back(
		new GravitationBody(merc_rad, 0.9f, DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f), DirectX::XMFLOAT4(0.3f, 0.2f, 0.2f, 1.0f), GravitationBody::PLANET_TYPE::CUBE)
	);
	gravBodies.back()->position = Vector3::Transform(Vector3(0.0f, 0.0f, accum_dist), GetRandomRotateTransform());
	gravBodies.back()->velocity = Vector3::Transform(Vector3(0.0f, 0.0f, GravitationBody::maxVelocity * 0.5), GetRandomRotateTransform());
	
	accum_dist += (merc_rad)+planet_gap + (jupiter_rad);

	GravitationBody* jupiter = new GravitationBody(jupiter_rad, 0.3f, DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f), DirectX::XMFLOAT4(0.58f, 0.29f, 0.0f, 1.0f), GravitationBody::PLANET_TYPE::SPHERE);
	gravBodies.push_back(jupiter);
	gravBodies.back()->position = Vector3::Transform(Vector3(0.0f, 0.0f, accum_dist), GetRandomRotateTransform());
	gravBodies.back()->velocity = Vector3::Transform(Vector3(0.0f, 0.0f, GravitationBody::maxVelocity * 0.5), GetRandomRotateTransform());

	accum_dist += (jupiter_rad)+planet_gap + (saturn_ring_rad + planet_gap);

	GravitationBody* saturn = new GravitationBody(saturn_rad, 0.8f, DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f), DirectX::XMFLOAT4(1.0f, 0.5f, 0.2f, 1.0f), GravitationBody::PLANET_TYPE::CUBE);
	GravitationBody* saturn_ring = new GravitationBody(saturn_ring_rad, 0.8f, DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f), DirectX::XMFLOAT4(0.58f, 0.29f, 0.0f, 1.0f), GravitationBody::PLANET_TYPE::RING);
	gravBodies.push_back(saturn);
	gravBodies.back()->position = Vector3::Transform(Vector3(0.0f, 0.0f, accum_dist), GetRandomRotateTransform());
	gravBodies.back()->velocity = Vector3::Transform(Vector3(0.0f, 0.0f, GravitationBody::maxVelocity * 0.5), GetRandomRotateTransform());


	accum_dist += (saturn_ring_rad + planet_gap) + planet_gap + (saturn_ring_rad + planet_gap);


	GravitationBody* uranus = new GravitationBody(uranus_rad, 0.8f, DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f), DirectX::XMFLOAT4(0.0f, 1.0f, 1.0f, 1.0f), GravitationBody::PLANET_TYPE::GEOSPHERE);
	gravBodies.push_back(uranus);
	gravBodies.back()->position = Vector3::Transform(Vector3(0.0f, 0.0f, accum_dist), GetRandomRotateTransform());
	gravBodies.back()->velocity = Vector3::Transform(Vector3(0.0f, 0.0f, GravitationBody::maxVelocity * 0.5), GetRandomRotateTransform());

	/*
	*/

	starBox = new StarBox(100.0f, 5.0f, Vector3::Zero, Vector4::Zero);
	scene.AddNode(starBox);

	// Добавление объектов на сцену
	for (auto body : gravBodies)
	{
		scene.AddNode(body);
	}

	physEngine = new SolarSystemPhysics(&scene);

	displayWindow = DisplayWindow(this, applicationName, hInstance, winWidth, winHeight);

	renderer = Renderer(&displayWindow);
	renderer.camera = Camera(winWidth * 1.0f / winHeight);

	//renderer.camera.SwitchToOrbitalMode(focusedBody->GetCenterLocation(), focusedBody->spinAxis, focusedBody->radius);

	for (auto node : scene.nodes)
	{
		node->LoadAndCompileShader(renderer.shaderManager);
		node->InitBuffers(renderer.resourceManager);
		node->camera = &(renderer.camera);
		//std::cout << "f1\n";
	}
	// Инициализация камеры
	//camera.SetPosition(DirectX::XMFLOAT3(0.0f, 10.0f, -20.0f));
	//camera.SetTarget(DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f));
	//camera.SetUp(DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f));

	InputDevice::getInstance().OnKeyPressed.AddRaw(this, &GravitationGame::HandleKeyDown);
	InputDevice::getInstance().MouseMove.AddRaw(this, &GravitationGame::HandleMouseMove);
}

Matrix GravitationGame::GetRandomRotateTransform() {
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<float> dis(0.0f, XM_PI * 2);

	float angleX = dis(gen);
	float angleY = dis(gen);
	float angleZ = dis(gen);

	Matrix rotationX = Matrix::CreateRotationX(angleX);
	Matrix rotationY = Matrix::CreateRotationY(angleY);
	Matrix rotationZ = Matrix::CreateRotationZ(angleZ);

	Matrix rotation = rotationX * rotationY * rotationZ;

	return rotation;
}

GravitationGame::~GravitationGame()
{
}

void GravitationGame::Run()
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

void GravitationGame::Update(float deltaTime)
{
	

	buttonTimer += deltaTime;


	for (size_t i = 0; i < gravBodies.size() - 1; i++)
	{
		for (size_t j = i + 1; j < gravBodies.size(); j++)
		{
			auto _force = gravBodies[i]->CalcForceBetween_noMass(*(gravBodies[j]));
			gravBodies[i]->acceleration += _force * gravBodies[j]->mass;
			gravBodies[j]->acceleration -= _force * gravBodies[i]->mass;
		}
	}

	physEngine->Update(deltaTime);

	if (focusedBody) {

		renderer.camera.Update(deltaTime, focusedBody->worldMat);
	}

	Matrix vpMat = renderer.camera.GetViewMatrix() * renderer.camera.GetProjectionMatrix();

	for (auto node : scene.nodes)
	{
		node->cb.wvpMat = node->worldMat * (XMMATRIX)vpMat;
	}

}

void GravitationGame::Render()
{
	// Отрисовка сцены
	renderer.RenderScene(scene);
}


void GravitationGame::HandleKeyDown(Keys key) {
	// Управление ракеткой игрока
	if (key == Keys::Up)
	{
		//std::cout << "MoveUp\n";
		renderer.camera.MoveUp(deltaTime);
	}
	if (key == Keys::Down)
	{
		//std::cout << "MoveDown\n";
		renderer.camera.MoveDown(deltaTime);
	}
	if (key == Keys::Right)
	{
		//std::cout << "MoveUp\n";
		renderer.camera.MoveRight(deltaTime);
	}
	if (key == Keys::Left)
	{
		//std::cout << "MoveDown\n";
		renderer.camera.MoveLeft(deltaTime);
	}
	if (key == Keys::E)
	{
		//std::cout << "MoveUp\n";
		renderer.camera.MoveForward(deltaTime);
	}
	if (key == Keys::Q)
	{
		//std::cout << "MoveDown\n";
		renderer.camera.MoveBackward(deltaTime);
	}
	if (key == Keys::W)
	{
		renderer.camera.RotatePitch(deltaTime);
	}
	if (key == Keys::S)
	{
		renderer.camera.RotatePitch(-deltaTime);
	}
	if (key == Keys::A)
	{
		renderer.camera.RotateYaw(-deltaTime);
	}
	if (key == Keys::D)
	{
		renderer.camera.RotateYaw(deltaTime);
	}
	if (key == Keys::D1)
	{
		focusedBody = gravBodies[0];
		renderer.camera.SwitchToOrbitalMode(focusedBody->GetCenterLocation(), focusedBody->spinAxis, focusedBody->radius);
	}
	if (key == Keys::D2)
	{
		focusedBody = gravBodies[1];
		renderer.camera.SwitchToOrbitalMode(focusedBody->GetCenterLocation(), focusedBody->spinAxis, focusedBody->radius);
	}
	if (key == Keys::D3)
	{
		focusedBody = gravBodies[2];
		renderer.camera.SwitchToOrbitalMode(focusedBody->GetCenterLocation(), focusedBody->spinAxis, focusedBody->radius);
	}
	if (key == Keys::D4)
	{
		focusedBody = gravBodies[3];
		renderer.camera.SwitchToOrbitalMode(focusedBody->GetCenterLocation(), focusedBody->spinAxis, focusedBody->radius);
	}
	if (key == Keys::OemComma)
	{
		if (buttonTimer > pressTime)
		{
			buttonTimer = 0.0f;
			focusedBodyIdx = (focusedBodyIdx - 1 + gravBodies.size()) % gravBodies.size();
			focusedBody = gravBodies[focusedBodyIdx];
			renderer.camera.SwitchToOrbitalMode(focusedBody->GetCenterLocation(), focusedBody->spinAxis, focusedBody->radius);
		}
	}
	if (key == Keys::OemPeriod)
	{
		if (buttonTimer > pressTime)
		{
			buttonTimer = 0.0f;
			focusedBodyIdx = (focusedBodyIdx + 1) % gravBodies.size();
			focusedBody = gravBodies[focusedBodyIdx];
			renderer.camera.SwitchToOrbitalMode(focusedBody->GetCenterLocation(), focusedBody->spinAxis, focusedBody->radius);
		}
	}
	if (key == Keys::B)
	{
		if (buttonTimer > pressTime)
		{
			buttonTimer = 0.0f;
			renderer.camera.SwitchProjection();
		}
	}
	if (key == Keys::Space)
	{
		if (buttonTimer > pressTime)
		{
			buttonTimer = 0.0f;
			focusedBody = nullptr;
			renderer.camera.SwitchToFPSMode();
		}
	}

}

void GravitationGame::HandleMouseMove(const InputDevice::MouseMoveEventArgs& args)
{
	renderer.camera.RotateYaw(deltaTime * args.Offset.x * 0.2);
	renderer.camera.RotatePitch(-deltaTime * args.Offset.y * 0.2);
}