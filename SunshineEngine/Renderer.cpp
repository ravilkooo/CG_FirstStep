#include "Renderer.h"

Renderer::Renderer() : device(nullptr), context(nullptr)
{
}

Renderer::Renderer(DisplayWindow* displayWin)
{
	PrevTime = std::chrono::steady_clock::now();
	totalTime = 0;

	LPCWSTR applicationName = L"SunshineEngine";
	HINSTANCE hInstance = GetModuleHandle(nullptr);

	this->displayWindow = displayWindow;

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

	hr = swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBuffer);
	if (FAILED(hr))
		throw std::runtime_error("Failed to get back buffer");

	// rtv

	hr = device->CreateRenderTargetView(backBuffer.Get(), nullptr, &renderTargetView);
	if (FAILED(hr))
		throw std::runtime_error("Failed to create Render Target View");


	// depth buffer
	pipelineState = PipelineState(GetDevice(), GetDeviceContext());
	pipelineState.SetDepthStencilState(true);
	
	D3D11_TEXTURE2D_DESC descDepth = {};
	descDepth.Width = screenWidth;
	descDepth.Height = screenHeight;
	descDepth.MipLevels = 1u;
	descDepth.ArraySize = 1u;
	descDepth.Format = DXGI_FORMAT_D32_FLOAT;
	descDepth.SampleDesc.Count = 1u;
	descDepth.SampleDesc.Quality = 0u;
	descDepth.Usage = D3D11_USAGE_DEFAULT;
	descDepth.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	descDepth.CPUAccessFlags = 0; // надо ли
	descDepth.MiscFlags = 0; // надо ли
	
	device->CreateTexture2D(&descDepth, nullptr, &pDepthStencil);

	D3D11_DEPTH_STENCIL_VIEW_DESC descDSV = {};
	descDSV.Format = DXGI_FORMAT_D32_FLOAT;
	descDSV.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	descDSV.Texture2D.MipSlice = 0u;
	device->CreateDepthStencilView(pDepthStencil, &descDSV, &pDSV);

	context->OMSetRenderTargets(1u, &renderTargetView, pDSV);

	resourceManager = ResourceManager(GetDevice());

	shaderManager = ShaderManager(GetDevice());

	pipelineState.SetRasterizerState(D3D11_CULL_BACK, D3D11_FILL_SOLID); // D3D11_CULL_NONE, D3D11_CULL_BACK, D3D11_FILL_SOLID, D3D11_FILL_WIREFRAME

	inputAssembler = InputAssembler(GetDevice(), GetDeviceContext());

	camera = Camera();

    return;
}


Renderer::~Renderer()
{

}


void Renderer::RenderScene(const Scene& scene)
{
	auto	curTime = std::chrono::steady_clock::now();
	float	deltaTime = std::chrono::duration_cast<std::chrono::microseconds>(curTime - PrevTime).count() / 1000000.0f;
	PrevTime = curTime;
	totalTime += deltaTime;
	totalTime = totalTime > 1.0f ? totalTime - 1.0f : totalTime;

	// 13. At the End of While(!isExitRequested) : Clear BackBuffer
	float color[] = { 0.1f, 0.1f, 0.1f, 1.0f };
	context->ClearRenderTargetView(renderTargetView, color);
	context->ClearDepthStencilView(pDSV, D3D11_CLEAR_DEPTH, 1.0f, 0u);

	D3D11_VIEWPORT viewport = {};
	viewport.Width = static_cast<float>(screenWidth);
	viewport.Height = static_cast<float>(screenHeight);
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.MinDepth = 0;
	viewport.MaxDepth = 1.0f;
	context->RSSetViewports(1, &viewport);

	for (SceneNode* node : scene.nodes) {
		DrawNode(node);
	}

	swapChain->Present(1, /*DXGI_PRESENT_DO_NOT_WAIT*/ 0);
}

void Renderer::DrawNode(SceneNode* node)
{
	inputAssembler.CreateInputLayout(node->IALayoutInputElements, node->numInputElements, node->vsBlob);
	inputAssembler.SetInputLayout();
	node->Draw(context, renderTargetView, pDSV);

	/*
	if (node->GetChildren().size() == 0) {
		// Draw this node
	}
	else {
		for (SceneNode* childNode : node->GetChildren())
		{
			childNode->Draw(context, renderTargetView, pDSV);
		}
	}
	*/
}

ID3D11Device* Renderer::GetDevice()
{
	return device.Get();
}

ID3D11DeviceContext* Renderer::GetDeviceContext()
{
	return context.Get();
}