#include <windows.h> // for FLOAT definition
//#include <xnamath.h>
#include <iostream>

#include <d3d.h>
#include <d3d11.h>
#include <d3dcompiler.h>
#include <directxmath.h>


#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")

//d3dll.lib;
//d3dxlld.lib;
//D3DCompiler.lib;
//Effectslld.lib;
//dxerr.lib;
//dxgi.lib;
//dxguid.lib;

using namespace std;
// Overload the "<<" operators so that we can use cout to
// output XMVECTOR objects.
ostream& operator<<(ostream& os, DirectX::FXMVECTOR v)
{
	DirectX::XMFLOAT3 dest;
	XMStoreFloat3(&dest, v);
	os << "(" << dest.x << ", " << dest.y << ", " << dest.z << ")";
	return os;
}

int test_0();
int test_1();

int main()
{
	test_1();
}

int test_0()
{
	cout.setf(ios_base::boolalpha);
	// Check support for SSE2 (Pentium4, AMD K8, and above).
	if (!DirectX::XMVerifyCPUSupport())
	{
		cout << "xna math not supported" << endl;
		return 0;
	}
	DirectX::XMVECTOR p = DirectX::XMVectorZero();
	DirectX::XMVECTOR q = DirectX::XMVectorSplatOne();
	DirectX::XMVECTOR u = DirectX::XMVectorSet(1.0f, 2.0f, 3.0f, 0.0f);
	DirectX::XMVECTOR v = DirectX::XMVectorReplicate(-2.0f);
	DirectX::XMVECTOR w = DirectX::XMVectorSplatZ(u);
	cout << "p = " << p << endl;
	cout << "q = " << q << endl;
	cout << "u = " << u << endl;
	cout << "v = " << v << endl;
	cout << "w = " << w << endl;
	return 0;
}

int test_1() {
	// Project onto unit sphere.
	DirectX::XMVECTOR p = DirectX::XMVectorSet(1.0f, 2.0f, 3.0f, 0.0f);
	cout << "p = " << p << endl;
	DirectX::XMVECTOR q = DirectX::XMVector3Normalize(p);
	cout << "q = " << q << endl;
	DirectX::XMVECTOR r = DirectX::XMVector4Normalize(p);
	cout << "r = " << r << endl;

	p = DirectX::XMVectorSet(1.0f, 2.0f, 3.0f, 2.0f);
	cout << "p = " << p << endl;
	q = DirectX::XMVector3Normalize(p);
	cout << "q = " << q << endl;
	r = DirectX::XMVector4Normalize(p);
	cout << "r = " << r << endl;

	return 0;
}