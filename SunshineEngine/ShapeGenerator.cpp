#include "ShapeGenerator.h"
#include <iostream>

DirectX::XMVECTOR NormalizeHomogeneousVector(DirectX::XMVECTOR vec) {
    // Получаем компонент w
    DirectX::XMVECTOR w = DirectX::XMVectorSplatW(vec);

    DirectX::XMVECTOR cartesian = DirectX::XMVectorDivide(vec, w);

    DirectX::XMVECTOR normalized = DirectX::XMVector3Normalize(cartesian);

    return DirectX::XMVectorSetW(normalized, 1.0f);
}

void CreateSimpleCubeMesh(float width, float height, float depth, DirectX::XMFLOAT4 col,
    Vertex** vertices, UINT* verticesNum, int** indices, UINT* indicesNum) {
    float w2 = 0.5f * width;
    float h2 = 0.5f * height;
    float d2 = 0.5f * depth;

    //DirectX::XMFLOAT4 col_1 = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
    //DirectX::XMFLOAT4 col_2 = DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);

    *vertices = (Vertex*)calloc(8, sizeof(Vertex));
    *verticesNum = 8;

    (*vertices)[0] = { DirectX::XMFLOAT3(-w2, -h2, -d2), col };
    (*vertices)[1] = { DirectX::XMFLOAT3(-w2, +h2, -d2), col };
    (*vertices)[2] = { DirectX::XMFLOAT3(+w2, +h2, -d2), col };
    (*vertices)[3] = { DirectX::XMFLOAT3(+w2, -h2, -d2), col };
    (*vertices)[4] = { DirectX::XMFLOAT3(-w2, -h2, +d2), col };
    (*vertices)[5] = { DirectX::XMFLOAT3(-w2, +h2, +d2), col };
    (*vertices)[6] = { DirectX::XMFLOAT3(+w2, +h2, +d2), col };
    (*vertices)[7] = { DirectX::XMFLOAT3(+w2, -h2, +d2), col };


    int _indices[36] = {
        0,1,2,
        //0,2,1,
        0,2,3,
        //0,3,2,

        4,6,5,
        //4,5,6,
        4,7,6,
        //4,6,7,

        4,5,1,
        4,1,0,

        3,2,6,
        //3,6,2,
        3,6,7,
        //3,7,6,

        1,5,6,
        1,6,2,

        4,0,3,
        4,3,7,
    };

    *indices = (int*)calloc(36, sizeof(int));
    *indicesNum = 36;

    for (size_t i = 0; i < 36; i++)
    {
        (*indices)[i] = _indices[i];
    }

    return;
}

void CreateSimpleGeosphereMesh(float radius, DirectX::XMFLOAT4 col,
    Vertex** vertices, UINT* verticesNum, int** indices, UINT* indicesNum) {
    

    //numSubdivisions = MathHelper::Min(numSubdivisions, 5u);

    const float X = 0.525731f;
    const float Z = 0.850651f;
    DirectX::XMFLOAT3 pos[12] =
    {
        DirectX::XMFLOAT3(-X, 0.0f, Z), DirectX::XMFLOAT3(X, 0.0f, Z),
        DirectX::XMFLOAT3(-X, 0.0f, -Z), DirectX::XMFLOAT3(X, 0.0f, -Z),
        DirectX::XMFLOAT3(0.0f, Z, X), DirectX::XMFLOAT3(0.0f, Z, -X),
        DirectX::XMFLOAT3(0.0f, -Z, X), DirectX::XMFLOAT3(0.0f, -Z, -X),
        DirectX::XMFLOAT3(Z, X, 0.0f), DirectX::XMFLOAT3(-Z, X, 0.0f),
        DirectX::XMFLOAT3(Z, -X, 0.0f), DirectX::XMFLOAT3(-Z, -X, 0.0f)
    };
    DWORD k[60] =
    {
    1,4,0, 4,9,0, 4,5,9, 8,5,4, 1,8,4,
    1,10,8, 10,3,8, 8,3,5, 3,2,5, 3,7,2,
    3,10,7, 10,6,7, 6,11,7, 6,0,11, 6,1,0,
    10,1,6, 11,0,9, 2,11,9, 5,2,9, 11,2,7
    };
    

    *vertices = (Vertex*)calloc(12, sizeof(Vertex));
    *verticesNum = 12;
    for (size_t i = 0; i < 12; i++) {
        (*vertices)[i] = { pos[i], col };
    }

    *indicesNum = 60;
    *indices = (int*)calloc(60, sizeof(int));
    for (size_t i = 0; i < 60; ++i)
        (*indices)[i] = k[i];

    
    //for (size_t i = 0; i < numSubdivisions; ++i)
    //    Subdivide(meshData);// Project vertices onto sphere and scale.
    for (size_t i = 0; i < 12; i++)
    {
        // Project onto unit sphere.
        DirectX::XMVECTOR _n = DirectX::XMVector3Normalize(DirectX::XMLoadFloat3(&(*vertices)[i].pos));
        // Project onto sphere.
        DirectX::XMVECTOR _p = DirectX::XMVectorScale (_n, radius);
        //DirectX::XMFLOAT4 _p_f4;
        DirectX::XMStoreFloat3(&((*vertices)[i].pos), _p);
        //std::cout << i << " " << (*vertices)[2 * i].x << ", ";
        //(*vertices)[2 * i] = _p_f4;
    }
    
    return;
}


void CreateSimpleSphereMesh(float radius, UINT sliceCount, UINT elevationCount,
    DirectX::XMFLOAT4 col,
    Vertex** vertices, UINT* verticesNum, int** indices, UINT* indicesNum) {

    sliceCount = max(sliceCount, 4);
    elevationCount = max(elevationCount, 1);

    *verticesNum = 2 + (2 * elevationCount + 1) * sliceCount;
    *vertices = (Vertex*)malloc(*verticesNum * sizeof(Vertex));

    float sliceStep = DirectX::XM_2PI / sliceCount;
    float elevationStep = DirectX::XM_PIDIV2 / (elevationCount + 1);


    UINT _offsetVertexIdx = 0;
    // top vertex
    (*vertices)[_offsetVertexIdx++] = { DirectX::XMFLOAT3(0.0f, radius, 0.0f), col };
    // other vertices
    for (UINT i = 1; i <= 2 * elevationCount + 1; ++i)
    {
        for (UINT j = 0; j < sliceCount; ++j) {
            (*vertices)[_offsetVertexIdx++] =
            { DirectX::XMFLOAT3(
                radius * sinf(elevationStep * i) * cosf(sliceStep * j),
                radius * cosf(elevationStep * i),
                radius * sinf(elevationStep * i) * sinf(sliceStep * j)
            ), col };
        }
    }
    // bottom vertex
    (*vertices)[_offsetVertexIdx++] = { DirectX::XMFLOAT3(0.0f, -radius, 0.0f), col };


    *indicesNum = 6 * sliceCount + 2 * 6 * elevationCount * sliceCount;
    //std::cout << *indicesNum << " << \n";
    *indices = (int*)malloc(*indicesNum * sizeof(int));

    UINT indexIndex = 0;

    for (UINT j = 0; j < sliceCount - 1; ++j) {
        (*indices)[indexIndex++] = 0;
        (*indices)[indexIndex++] = j + 2;
        (*indices)[indexIndex++] = j + 1;
    }
    
    (*indices)[indexIndex++] = 0;
    (*indices)[indexIndex++] = 1;
    (*indices)[indexIndex++] = sliceCount;


    for (UINT i = 0; i < 2 * elevationCount; ++i) {
        UINT startIndex = 1 + i * sliceCount;
        UINT nextStartIndex = startIndex + sliceCount;
        for (UINT j = 0; j < sliceCount - 1; ++j) {
            
            (*indices)[indexIndex++] = startIndex + j;
            (*indices)[indexIndex++] = startIndex + j + 1;
            (*indices)[indexIndex++] = nextStartIndex + j;

            (*indices)[indexIndex++] = startIndex + j + 1;
            (*indices)[indexIndex++] = nextStartIndex + j + 1;
            (*indices)[indexIndex++] = nextStartIndex + j;
        }

        (*indices)[indexIndex++] = startIndex + sliceCount - 1;
        (*indices)[indexIndex++] = startIndex;
        (*indices)[indexIndex++] = nextStartIndex + sliceCount - 1;

        (*indices)[indexIndex++] = startIndex;
        (*indices)[indexIndex++] = nextStartIndex;
        (*indices)[indexIndex++] = nextStartIndex + sliceCount - 1;
    }
    
    
    UINT bottomIndex = _offsetVertexIdx - 1;
    UINT startIndex = 1 + 2 * elevationCount * sliceCount;
    for (UINT j = 0; j < sliceCount-1; ++j) {
        (*indices)[indexIndex++] = bottomIndex;
        (*indices)[indexIndex++] = startIndex + j;
        (*indices)[indexIndex++] = startIndex + j + 1;
    }

    (*indices)[indexIndex++] = bottomIndex;
    (*indices)[indexIndex++] = startIndex + sliceCount - 1;
    (*indices)[indexIndex++] = startIndex;

    return;
}

void CreateRingMesh(float radius, float thickness, float width,
    UINT sliceCount,
    DirectX::XMFLOAT4 col,
    Vertex** vertices, UINT* verticesNum, int** indices, UINT* indicesNum)
{
    sliceCount = max(sliceCount, 4);
    thickness = max(0.0001, thickness);

    *verticesNum = 4 * sliceCount;
    *vertices = (Vertex*)malloc(*verticesNum * sizeof(Vertex));

    float sliceStep = DirectX::XM_2PI / sliceCount;

    UINT _offsetVertexIdx = 0;
    // top vertices
    
    {
        float half_thickness = 0.5 * thickness;

        for (UINT j = 0; j < sliceCount; ++j) {
            (*vertices)[_offsetVertexIdx++] =
            { DirectX::XMFLOAT3(
                radius * cosf(sliceStep * j),
                half_thickness,
                radius *  sinf(sliceStep * j)
            ), col };
        }

        float outer_radius = radius + width;

        for (UINT j = 0; j < sliceCount; ++j) {
            (*vertices)[_offsetVertexIdx++] =
            { DirectX::XMFLOAT3(
                outer_radius * cosf(sliceStep * j),
                half_thickness,
                outer_radius * sinf(sliceStep * j)
            ), col };
        }

        for (UINT j = 0; j < sliceCount; ++j) {
            (*vertices)[_offsetVertexIdx++] =
            { DirectX::XMFLOAT3(
                outer_radius * cosf(sliceStep * j),
                -half_thickness,
                outer_radius * sinf(sliceStep * j)
            ), col };
        }

        for (UINT j = 0; j < sliceCount; ++j) {
            (*vertices)[_offsetVertexIdx++] =
            { DirectX::XMFLOAT3(
                radius * cosf(sliceStep * j),
                -half_thickness,
                radius * sinf(sliceStep * j)
            ), col };
        }

    }


    *indicesNum = 6 * 4 * sliceCount;
    //std::cout << *indicesNum << " << \n";
    *indices = (int*)malloc(*indicesNum * sizeof(int));

    UINT indexIndex = 0;
    for (UINT i = 0; i < 3; i++)
    {
        UINT startIndex = i * sliceCount;
        UINT nextStartIndex = startIndex + sliceCount;
        for (UINT j = 0; j < sliceCount - 1; ++j) {

            (*indices)[indexIndex++] = startIndex + j;
            (*indices)[indexIndex++] = startIndex + j + 1;
            (*indices)[indexIndex++] = nextStartIndex + j;

            (*indices)[indexIndex++] = startIndex + j + 1;
            (*indices)[indexIndex++] = nextStartIndex + j + 1;
            (*indices)[indexIndex++] = nextStartIndex + j;
        }

        (*indices)[indexIndex++] = startIndex + sliceCount - 1;
        (*indices)[indexIndex++] = startIndex;
        (*indices)[indexIndex++] = nextStartIndex + sliceCount - 1;

        (*indices)[indexIndex++] = startIndex;
        (*indices)[indexIndex++] = nextStartIndex;
        (*indices)[indexIndex++] = nextStartIndex + sliceCount - 1;

    }

    {
        UINT startIndex = 3 * sliceCount;
        UINT nextStartIndex = 0;
        for (UINT j = 0; j < sliceCount - 1; ++j) {

            (*indices)[indexIndex++] = startIndex + j;
            (*indices)[indexIndex++] = startIndex + j + 1;
            (*indices)[indexIndex++] = nextStartIndex + j;

            (*indices)[indexIndex++] = startIndex + j + 1;
            (*indices)[indexIndex++] = nextStartIndex + j + 1;
            (*indices)[indexIndex++] = nextStartIndex + j;
        }

        (*indices)[indexIndex++] = startIndex + sliceCount - 1;
        (*indices)[indexIndex++] = startIndex;
        (*indices)[indexIndex++] = nextStartIndex + sliceCount - 1;

        (*indices)[indexIndex++] = startIndex;
        (*indices)[indexIndex++] = nextStartIndex;
        (*indices)[indexIndex++] = nextStartIndex + sliceCount - 1;

    }

    return;
}
