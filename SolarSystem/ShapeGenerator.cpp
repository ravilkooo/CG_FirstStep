#include "ShapeGenerator.h"

void CreateSimpleCubeMesh(float width, float height, float depth,
    DirectX::XMFLOAT4** vertices, int** indices, DirectX::XMFLOAT4 col) {
    float w2 = 0.5f * width;
    float h2 = 0.5f * height;
    float d2 = 0.5f * depth;

    DirectX::XMFLOAT4 col_1 = DirectX::XMFLOAT4(1.0f, 1.0f, 1.0f, 1.0f);
    DirectX::XMFLOAT4 col_2 = DirectX::XMFLOAT4(0.0f, 0.0f, 1.0f, 1.0f);

    *vertices = (DirectX::XMFLOAT4*)calloc(16, sizeof(DirectX::XMFLOAT4));


    (*vertices)[0] = DirectX::XMFLOAT4(-w2, -h2, -d2, 1.0f);
    (*vertices)[2] = DirectX::XMFLOAT4(-w2, +h2, -d2, 1.0f);
    (*vertices)[4] = DirectX::XMFLOAT4(+w2, +h2, -d2, 1.0f);
    (*vertices)[6] = DirectX::XMFLOAT4(+w2, -h2, -d2, 1.0f);
    (*vertices)[8] = DirectX::XMFLOAT4(-w2, -h2, +d2, 1.0f);
    (*vertices)[10] = DirectX::XMFLOAT4(-w2, +h2, +d2, 1.0f);
    (*vertices)[12] = DirectX::XMFLOAT4(+w2, +h2, +d2, 1.0f);
    (*vertices)[14] = DirectX::XMFLOAT4(+w2, -h2, +d2, 1.0f);
    for (size_t i = 0; i < 8; i++)
    {
        (*vertices)[2 * i + 1] = DirectX::XMFLOAT4(i / 2 < 2 ? col_1 : col);
    }

    *indices = (int*)calloc(36, sizeof(int));

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

    std::cout << "Hi!\n";

    for (size_t i = 0; i < 36; i++)
    {
        (*indices)[i] = _indices[i];
    }

    return;
}


/*
void GeometryGenerator::CreateGeosphere(float radius, UINT numSubdivisions, MeshData& meshData)
{
// Put a cap on the number of subdivisions.
numSubdivisions = MathHelper::Min(numSubdivisions, 5u);
// Approximate a sphere by tessellating an icosahedron.
const float X = 0.525731f;
const float Z = 0.850651f;
XMFLOAT3 pos[12] =
{
XMFLOAT3(-X, 0.0f, Z), XMFLOAT3(X, 0.0f, Z),
XMFLOAT3(-X, 0.0f, -Z), XMFLOAT3(X, 0.0f, -Z),
XMFLOAT3(0.0f, Z, X), XMFLOAT3(0.0f, Z, -X),
XMFLOAT3(0.0f, -Z, X), XMFLOAT3(0.0f, -Z, -X),
XMFLOAT3(Z, X, 0.0f), XMFLOAT3(-Z, X, 0.0f),
XMFLOAT3(Z, -X, 0.0f), XMFLOAT3(-Z, -X, 0.0f)
};
DWORD k[60] =
{
1,4,0, 4,9,0, 4,5,9, 8,5,4, 1,8,4,
1,10,8, 10,3,8, 8,3,5, 3,2,5, 3,7,2,
3,10,7, 10,6,7, 6,11,7, 6,0,11, 6,1,0,
10,1,6, 11,0,9, 2,11,9, 5,2,9, 11,2,7
};
meshData.Vertices.resize(12);
meshData.Indices.resize(60);
for(size_t i = 0; i < 12; ++i)
meshData.Vertices[i].Position = pos[i];
for(size_t i = 0; i < 60; ++i)
meshData.Indices[i] = k[i];
for(size_t i = 0; i < numSubdivisions; ++i)
Subdivide(meshData);// Project vertices onto sphere and scale.
for(size_t i = 0; i < meshData.Vertices.size(); ++i)
{
// Project onto unit sphere.
XMVECTOR n = XMVector3Normalize(XMLoadFloat3(
&meshData.Vertices[i].Position));
// Project onto sphere.
XMVECTOR p = radius*n;
XMStoreFloat3(&meshData.Vertices[i].Position, p);
XMStoreFloat3(&meshData.Vertices[i].Normal, n);
// Derive texture coordinates from spherical coordinates.
float theta = MathHelper::AngleFromXY(
meshData.Vertices[i].Position.x,
meshData.Vertices[i].Position.z);
float phi = acosf(meshData.Vertices[i].Position.y / radius);
meshData.Vertices[i].TexC.x = theta/XM_2PI;
meshData.Vertices[i].TexC.y = phi/XM_PI;
// Partial derivative of P with respect to theta
meshData.Vertices[i].TangentU.x = -radius*sinf(phi)*sinf(theta);
meshData.Vertices[i].TangentU.y = 0.0f;
meshData.Vertices[i].TangentU.z = +radius*sinf(phi)*cosf(theta);
XMVECTOR T = XMLoadFloat3(&meshData.Vertices[i].TangentU);
XMStoreFloat3(&meshData.Vertices[i].TangentU,
XMVector3Normalize(T));
}
}
*/