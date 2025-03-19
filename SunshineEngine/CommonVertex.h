#pragma once
#include <DirectXMath.h>

class CommonVertex
{
public:
    CommonVertex() {}
    CommonVertex(float x, float y, float z, float r, float g, float b, float a)
        : pos(x, y, z), color(r, g, b, a) {
    }
    CommonVertex(float x, float y, float z, float r, float g, float b)
        : pos(x, y, z), color(r, g, b, 1) {
    }
    CommonVertex(float x, float y, float z)
        : pos(x, y, z), color(0, 0, 0, 1) {
    }
    CommonVertex(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT4 color)
        : pos(pos), color(color) {
    }
    CommonVertex(const CommonVertex& cv)
        : pos(cv.pos), color(cv.color) {
    }

    CommonVertex& operator=(const CommonVertex& src) {
        this->pos = src.pos;
        this->color = src.color;
        return *this;
    }
    bool operator==(const CommonVertex& rhs) const {

    }
    bool operator!=(const CommonVertex& rhs) const {

    }

    DirectX::XMFLOAT3 pos = { 0, 0, 0 };
    DirectX::XMFLOAT4 color = { 0, 0, 0, 1 };
};

