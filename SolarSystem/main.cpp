#include <windows.h>
#include <WinUser.h>
#include <wrl.h>
#include <iostream>
#include <d3d.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxmath.h>
#include <chrono>

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")

#include "SolarSystemGame.h"
#include "GravitationGame.h"

int main()
{
	GravitationGame game = GravitationGame();
	//SolarSystemGame game = SolarSystemGame();
	game.Run();

	return 0;
}