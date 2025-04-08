#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")

#include "ShadowGame.h"

int main()
{
	ShadowGame game = ShadowGame();
	//SolarSystemGame game = SolarSystemGame();
	game.Run();

	return 0;
}