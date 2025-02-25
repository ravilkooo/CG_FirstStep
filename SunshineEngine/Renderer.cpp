#include "Renderer.h"

Renderer::Renderer() : device(nullptr), context(nullptr)
{
}

Renderer::Renderer(DisplayWindow* displayWin)
{
	Initialize(displayWin);
}

Renderer::~Renderer()
{
    //if (context) context->Release();
    //if (device) device->Release();
}

bool Renderer::Initialize(DisplayWindow* displayWin)
{
	PrevTime = std::chrono::steady_clock::now();
	totalTime = 0;

	LPCWSTR applicationName = L"SunshineEngine";
	HINSTANCE hInstance = GetModuleHandle(nullptr);

	this->displayWindow = displayWindow;

	/*HRESULT hr = D3D11CreateDevice(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		0,
		featureLevels,
		1,
		D3D11_SDK_VERSION,
		&device,
		nullptr,
		&context
	);
	if (FAILED(hr))
		throw std::runtime_error("Failed to create D3D11 device");*/

	// swapChain

	DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
	swapChainDesc.BufferCount = 2;
	swapChainDesc.BufferDesc.Width = screenWidth;
	swapChainDesc.BufferDesc.Height = screenHeight;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.OutputWindow = displayWin->hWnd;
	swapChainDesc.Windowed = true;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;

	HRESULT hr = D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		D3D11_CREATE_DEVICE_DEBUG,
		featureLevels,
		1,
		D3D11_SDK_VERSION,
		&swapChainDesc,
		&swapChain,
		&device,
		nullptr,
		&context);
	if (FAILED(hr))
		throw std::runtime_error("Failed to create Device with Swap Chain");

	// backBuffer

	Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;

	hr = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBuffer);
	if (FAILED(hr))
		throw std::runtime_error("Failed to get back buffer");

	// rtv

	hr = device->CreateRenderTargetView(backBuffer.Get(), nullptr, &renderTargetView);
	if (FAILED(hr))
		throw std::runtime_error("Failed to create Render Target View");

	// resourceManagers for buffers

	resourceManager = ResourceManager(GetDevice());

	// Load and compile shaders (move to other section?)
	shaderManager = ShaderManager(GetDevice());

	inputAssembler = InputAssembler(GetDevice(), GetDeviceContext());

	pipelineState = PipelineState(GetDevice(), GetDeviceContext());

    return true;
}

void Renderer::RenderScene(const Scene& scene)
{
	auto	curTime = std::chrono::steady_clock::now();
	float	deltaTime = std::chrono::duration_cast<std::chrono::microseconds>(curTime - PrevTime).count() / 1000000.0f;
	PrevTime = curTime;
	totalTime += deltaTime;
	totalTime = totalTime > 1.0f ? totalTime - 1.0f : totalTime;

	// 13. At the End of While(!isExitRequested) : Clear BackBuffer
	float color[] = { totalTime, 0.1f, 0.1f, 1.0f };
	context->ClearRenderTargetView(renderTargetView, color);

	/*struct Quadic {
		int indices[6] = { 0,1,2, 1,0,3 };
		DirectX::XMFLOAT4 points[8] = {
			DirectX::XMFLOAT4(0.25f, 0.25f, 0.25f, 1.0f),	DirectX::XMFLOAT4(1.0f, 0.0f, 0.0f, 1.0f),
			DirectX::XMFLOAT4(-0.25f, -0.25f, 0.25f, 1.0f),	DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f),
			DirectX::XMFLOAT4(0.25f, -0.25f, 0.25f, 1.0f),	DirectX::XMFLOAT4(0.0f, 1.0f, 0.0f, 1.0f),
			DirectX::XMFLOAT4(-0.25f, 0.25f, 0.25f, 1.0f),	DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f),
		};
	} quad1, quad2;

	for (size_t i = 0; i < 4; i++)
	{
		quad2.points[2 * i].x += 0.5;
	}

	Quadic quads[2] = { quad1, quad2 };*/


	D3D11_VIEWPORT viewport = {};
	viewport.Width = static_cast<float>(screenWidth);
	viewport.Height = static_cast<float>(screenHeight);
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.MinDepth = 0;
	viewport.MaxDepth = 1.0f;

	for (SceneNode* node : scene.nodes) {


		// Input layout handler

		// Описываем какие элементы будут на входе
		D3D11_INPUT_ELEMENT_DESC inputElements[] = {
			D3D11_INPUT_ELEMENT_DESC {
				"POSITION",
				0,
				DXGI_FORMAT_R32G32B32A32_FLOAT,
				0,
				0,
				D3D11_INPUT_PER_VERTEX_DATA,
				0},
			D3D11_INPUT_ELEMENT_DESC {
				"COLOR",
				0,
				DXGI_FORMAT_R32G32B32A32_FLOAT,
				0,
				D3D11_APPEND_ALIGNED_ELEMENT,
				D3D11_INPUT_PER_VERTEX_DATA,
				0}
		};

		inputAssembler.CreateInputLayout(inputElements, node->vsBlob);

		pipelineState.SetRasterizerState(D3D11_CULL_NONE, D3D11_FILL_SOLID);

		// 10. Setup Rasterizer Stage and Viewport

		context->RSSetViewports(1, &viewport);

		// 8. Setup the IA stage

		inputAssembler.SetInputLayout();
		inputAssembler.SetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		node->Draw(context, renderTargetView);
	}

	// зачем эта строчка? 
	context->OMSetRenderTargets(0, nullptr, nullptr);

	// 15. At the End of While (!isExitRequested): Present the Result
	swapChain->Present(1, /*DXGI_PRESENT_DO_NOT_WAIT*/ 0);
}

ID3D11Device* Renderer::GetDevice()
{
	return device.Get();
}

ID3D11DeviceContext* Renderer::GetDeviceContext()
{
	return context.Get();
}