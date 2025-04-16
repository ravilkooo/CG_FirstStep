#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")

#include "DeferredGame.h"

int main() {
	DeferredGame game = DeferredGame();
	game.Run();

	return 0;
}