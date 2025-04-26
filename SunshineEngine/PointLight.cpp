#include "PointLight.h"

PointLight::PointLight(ID3D11Device* device, Vector3 position,
    float range, Vector3 att, Vector4 ambient, Vector4 diffuse, Vector4 specular)
{
    this->ambient;

    float c = fmax(fmax(diffuse.x, diffuse.y), diffuse.z) / att.y;
    range = max(range, (8.0f * sqrtf(c) + 1.0f));

    pointLightData = {
        diffuse, specular, position, range,
        att, 0.0f
    };

    CreateSimpleSphereMesh(range, 10, 5,
        diffuse,
        &vertices, &verticesNum, &indices, &indicesNum);

    // LightPass
    {
        RenderTechnique* lightPass = new RenderTechnique("LightPass");
        lightPass->AddBind(new Bind::Topology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
        lightPass->AddBind(new Bind::IndexBuffer(device, indices, indicesNum));
        // AddStaticBind(texture);
        Bind::VertexShader* vertexShaderB = new Bind::VertexShader(device, L"./Shaders/PointLightVShader.hlsl");
        lightPass->AddBind(vertexShaderB);


        numInputElements = 1;
        IALayoutInputElements = (D3D11_INPUT_ELEMENT_DESC*)malloc(numInputElements * sizeof(D3D11_INPUT_ELEMENT_DESC));

        IALayoutInputElements[0] =
            D3D11_INPUT_ELEMENT_DESC{
                "POSITION",
                0,
                DXGI_FORMAT_R32G32B32_FLOAT,
                0,
                0,
                D3D11_INPUT_PER_VERTEX_DATA,
                0 };

        lightPass->AddBind(new Bind::InputLayout(device, IALayoutInputElements, numInputElements, vertexShaderB->GetBytecode()));


        lightPass->AddBind(new Bind::PixelShader(device, L"./Shaders/PointLightPShader.hlsl"));

        /*
        D3D11_RASTERIZER_DESC rastDesc = CD3D11_RASTERIZER_DESC(CD3D11_DEFAULT{});
        rastDesc.CullMode = D3D11_CULL_BACK;
        rastDesc.FillMode = D3D11_FILL_SOLID;
        lightPass->AddBind(new Bind::Rasterizer(device, rastDesc));
        */
        lightPass->AddBind(new Bind::VertexBuffer(device, vertices, verticesNum, sizeof(CommonVertex)));
        lightPass->AddBind(new Bind::TransformCBuffer(device, this, 0u));

        pointLightPBuffer = new Bind::PixelConstantBuffer<PointLightPCB>(device, pointLightData, 1u);
        lightPass->AddBind(pointLightPBuffer);

        techniques.insert({ "LightPass", lightPass });
    }
}

D3D11_DEPTH_STENCIL_DESC PointLight::GetDepthStencilDesc(LightObject::LightPosition lightPos)
{
    D3D11_DEPTH_STENCIL_DESC dsDesc = {};
    if (lightPos == LightPosition::INSIDE) {
        dsDesc.DepthEnable = TRUE;
        dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
        dsDesc.DepthFunc = D3D11_COMPARISON_GREATER;
    }
    else if (lightPos == LightPosition::FILL || lightPos == LightPosition::BEHIND_NEAR_PLANE) {
        dsDesc.DepthEnable = TRUE;
        dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
        dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
    }
    else if (lightPos == LightPosition::INTERSECT_FAR_PLANE) {
        dsDesc.DepthEnable = TRUE;
        dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
        dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
    }
    else { // I dont know why, just 
        dsDesc.DepthEnable = TRUE;
        dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
        dsDesc.DepthFunc = D3D11_COMPARISON_GREATER;
    }
    return dsDesc;
}

D3D11_RASTERIZER_DESC PointLight::GetRasterizerDesc(LightObject::LightPosition lightPos)
{
    D3D11_RASTERIZER_DESC rasterDesc = {};
    if (lightPos == LightPosition::INSIDE) {
        rasterDesc.CullMode = D3D11_CULL_FRONT;
        rasterDesc.FillMode = D3D11_FILL_SOLID;
    }
    else if (lightPos == LightPosition::FILL || lightPos == LightPosition::BEHIND_NEAR_PLANE) {
        rasterDesc.CullMode = D3D11_CULL_NONE;
        rasterDesc.FillMode = D3D11_FILL_SOLID;
    }
    else if (lightPos == LightPosition::INTERSECT_FAR_PLANE) {
        rasterDesc.CullMode = D3D11_CULL_BACK;
        rasterDesc.FillMode = D3D11_FILL_SOLID;
    }
    else { // I dont know why, just 
        rasterDesc.CullMode = D3D11_CULL_FRONT;
        rasterDesc.FillMode = D3D11_FILL_SOLID;
    }
    return rasterDesc;
}

LightObject::LightPosition PointLight::GetLightPositionInFrustum(Camera* camera)
{
    if (IsFrustumInsideOfLight(camera))
        return LightPosition::FILL;


    Camera::FrustumPlanes planes = camera->GetFrustumPlanes();
    XMVECTOR lightPosition = XMLoadFloat3(&(pointLightData.Position));

    bool isOutside = false;
    bool intersectsFarPlane = false;
    bool behindNearPlane = false;

    // �������� ������ ��������� ��������
    XMVECTOR planesArray[] = { planes.Near, planes.Far, planes.Left, planes.Right, planes.Top, planes.Bottom };

    for (int i = 0; i < 6; i++) {
        // ���������� �� ������ ����� �� ���������
        float distance = XMVectorGetX(XMPlaneDotCoord(planesArray[i], lightPosition));

        // ���� ���������� ������ -radius, ����� ��������� ��� ���������
        if (distance <= -pointLightData.Range) {
            return LightPosition::OUTSIDE;
        }

        // ���� �� ������� ����������
        if (i == 0 && distance < 0) {
            behindNearPlane = true;
        }

        // ���� ���������� ������� ���������
        if (i == 1 && abs(distance) < pointLightData.Range) {
            intersectsFarPlane = true;
        }

        // ���� ���������� ���� �� ���� ���������, �� �� ��������� ���
        if (abs(distance) < pointLightData.Range) {
            isOutside = true;
        }
    }

    if (intersectsFarPlane) {
        return LightPosition::INTERSECT_FAR_PLANE;
    }

    if (behindNearPlane) {
        return LightPosition::BEHIND_NEAR_PLANE;
    }

    return isOutside ? LightPosition::INSIDE : LightPosition::OUTSIDE;

}

bool PointLight::IsFrustumInsideOfLight(Camera* camera)
{
    Camera::FrustumCorners frustum = camera->GetFrustumCorners();
    for (int i = 0; i < 4; ++i) {
        XMVECTOR vecToCorner = XMVectorSubtract(frustum.Near[i], Vector3(pointLightData.Position));
        float distance = XMVectorGetX(XMVector3Length(vecToCorner));
        if (distance > pointLightData.Range) return false;
    }
    for (int i = 0; i < 4; ++i) {
        XMVECTOR vecToCorner = XMVectorSubtract(frustum.Far[i], Vector3(pointLightData.Position));
        float distance = XMVectorGetX(XMVector3Length(vecToCorner));
        if (distance > pointLightData.Range) return false;
    }
    return true;
}

void PointLight::Update(float deltaTime) {
    return;
}

Vector3 PointLight::GetCenterLocation() {
    return pointLightData.Position;
}