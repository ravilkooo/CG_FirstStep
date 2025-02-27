#include "DisplayWindow.h"

DisplayWindow::DisplayWindow() {

}

DisplayWindow::DisplayWindow(LPCWSTR applicationName, HINSTANCE hInstance, int screenWidth, int screenHeight)
{
	Initialize(applicationName, hInstance, screenWidth, screenHeight);
}

void DisplayWindow::Initialize(LPCWSTR applicationName, HINSTANCE hInstance, int screenWidth, int screenHeight)
{
	inputHandler = new InputHandler();

	WNDCLASSEX wc;

	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = WndProc_RawInput;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(nullptr, IDI_WINLOGO);
	wc.hIconSm = wc.hIcon;
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc.hbrBackground = static_cast<HBRUSH>(GetStockObject(BLACK_BRUSH));
	wc.lpszMenuName = nullptr;
	wc.lpszClassName = applicationName;
	wc.cbSize = sizeof(WNDCLASSEX);

	// Register the window class.
	RegisterClassEx(&wc);

	this->screenWidth = screenWidth;
	this->screenHeight = screenHeight;

	RECT windowRect = { 0, 0, static_cast<LONG>(screenWidth), static_cast<LONG>(screenHeight) };
	AdjustWindowRect(&windowRect, WS_OVERLAPPEDWINDOW, FALSE);

	auto dwStyle = WS_SYSMENU | WS_CAPTION | WS_MINIMIZEBOX | WS_THICKFRAME;

	auto posX = (GetSystemMetrics(SM_CXSCREEN) - screenWidth) / 2;
	auto posY = (GetSystemMetrics(SM_CYSCREEN) - screenHeight) / 2;

	hWnd = CreateWindowEx(WS_EX_APPWINDOW, applicationName, applicationName,
		dwStyle,
		posX, posY,
		windowRect.right - windowRect.left,
		windowRect.bottom - windowRect.top,
		nullptr, nullptr, hInstance, nullptr);
	SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(inputHandler));

	ShowWindow(hWnd, SW_SHOW);
	SetForegroundWindow(hWnd);
	SetFocus(hWnd);

	RegisterRawInput(hWnd);

	ShowCursor(true);

	//std::cout << "Zdarova vsem!\n";
}

LRESULT CALLBACK DisplayWindow::WndProc(HWND hwnd, UINT umessage, WPARAM wparam, LPARAM lparam)
{
	switch (umessage)
	{
	case WM_KEYDOWN:
	{
		// If a key is pressed send it to the input object so it can record that state.
		//std::cout << "Key: " << static_cast<unsigned int>(wparam) << std::endl;

		if (static_cast<unsigned int>(wparam) == 27) PostQuitMessage(0);
		return 0;
	}
	default:
	{
		return DefWindowProc(hwnd, umessage, wparam, lparam);
	}
	}
}


LRESULT CALLBACK DisplayWindow::WndProc_RawInput(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	InputHandler* pInputHandler = reinterpret_cast<InputHandler*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));

	switch (message)
	{
	case WM_INPUT:
	{
		HRAWINPUT hRawInput = reinterpret_cast<HRAWINPUT>(lParam);
		HandleRawInput(hRawInput, pInputHandler);
		break;
	}
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}


void DisplayWindow::HandleRawInput(HRAWINPUT hRawInput, InputHandler* inputHandler)
{
	UINT dwSize = 0;
	GetRawInputData(hRawInput, RID_INPUT, nullptr, &dwSize, sizeof(RAWINPUTHEADER));

	std::vector<BYTE> buffer(dwSize);
	if (GetRawInputData(hRawInput, RID_INPUT, buffer.data(), &dwSize, sizeof(RAWINPUTHEADER)) != dwSize)
	{
		std::cerr << "Failed to get raw input data" << std::endl;
		return;
	}

	RAWINPUT* raw = reinterpret_cast<RAWINPUT*>(buffer.data());

	if (raw->header.dwType == RIM_TYPEKEYBOARD)
	{
		RAWKEYBOARD& keyboard = raw->data.keyboard;
		bool isPressed = !(keyboard.Flags & RI_KEY_BREAK);
		inputHandler->UpdateKeyState(keyboard.VKey, isPressed);
	}
}

void DisplayWindow::RegisterRawInput(HWND hWnd)
{
	RAWINPUTDEVICE rid[1];

	rid[0].usUsagePage = 0x01;
	rid[0].usUsage = 0x06;
	rid[0].dwFlags = 0;
	rid[0].hwndTarget = hWnd;

	if (RegisterRawInputDevices(rid, 1, sizeof(rid[0])) == FALSE)
	{
		throw std::runtime_error("Failed to register raw input devices");
	}
}
