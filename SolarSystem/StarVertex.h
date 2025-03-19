#pragma once
#include <DirectXMath.h>

class StarVertex
{
    StarVertex() {}
    StarVertex(float x, float y, float z, float r, float g, float b, float a)
        : pos(x, y, z), color(r, g, b, a) {
    }
    StarVertex(float x, float y, float z, float r, float g, float b)
        : pos(x, y, z), color(r, g, b, 1) {
    }
    StarVertex(float x, float y, float z)
        : pos(x, y, z), color(0, 0, 0, 1) {
    }

    DirectX::XMFLOAT3 pos = { 0, 0, 0 };
    DirectX::XMFLOAT4 color = { 0, 0, 0, 1 };
};

