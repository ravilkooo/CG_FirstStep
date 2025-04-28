#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")

#include "KatamariDefGame.h"

int main()
{
	KatamariDefGame game = KatamariDefGame();
	game.Run();

	return 0;
}