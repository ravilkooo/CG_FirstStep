#include "CollectibleObject.h"


CollectibleObject::CollectibleObject(ID3D11Device* device, float radius, const DirectX::XMFLOAT3& position)
    : radius(radius), initialPosition(position)
{
    DirectX::XMFLOAT4 color(0.2f, 0.8f, 0.3f, 1.0f);
    CreateSimpleSphereMesh(radius, 20, 10, color,
        &vertices, &verticesNum, &indices, &indicesNum);
    worldMat = Matrix::CreateTranslation(position);


    {
        RenderTechnique* shadowPass = new RenderTechnique("DL_ShadowMapPass");
        shadowPass->AddBind(new Bind::Topology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
        shadowPass->AddBind(new Bind::VertexBuffer(device, vertices, verticesNum, sizeof(CommonVertex)));
        shadowPass->AddBind(new Bind::IndexBuffer(device, indices, indicesNum));
        shadowPass->AddBind(new Bind::TransformCBuffer(device, this, 1u));

        techniques.insert({ "DL_ShadowMapPass", shadowPass });
    }
    {
        RenderTechnique* colorPass = new RenderTechnique("MainColorPass");

        colorPass->AddBind(new Bind::Topology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
        colorPass->AddBind(new Bind::VertexBuffer(device, vertices, verticesNum, sizeof(CommonVertex)));
        colorPass->AddBind(new Bind::IndexBuffer(device, indices, indicesNum));
        colorPass->AddBind(new Bind::TextureB(device, "models\\Textures\\basketballskin.dds", aiTextureType_DIFFUSE, 1u));
        vertexShaderB = new Bind::VertexShader(device, L"./Shaders/ImportVShader.hlsl");
        colorPass->AddBind(vertexShaderB);

        {
            numInputElements = 3;
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

            IALayoutInputElements[1] =
                D3D11_INPUT_ELEMENT_DESC{
                    "COLOR",
                    0,
                    DXGI_FORMAT_R32G32B32A32_FLOAT,
                    0,
                    D3D11_APPEND_ALIGNED_ELEMENT,
                    D3D11_INPUT_PER_VERTEX_DATA,
                    0 };
            IALayoutInputElements[2] =
                D3D11_INPUT_ELEMENT_DESC{
                    "TEXCOORD",
                    0,
                    DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT,
                    0,
                    D3D11_APPEND_ALIGNED_ELEMENT, // 28,
                    D3D11_INPUT_PER_VERTEX_DATA,
                    0 };

        }

        colorPass->AddBind(new Bind::InputLayout(device, IALayoutInputElements, numInputElements, vertexShaderB->GetBytecode()));
        colorPass->AddBind(new Bind::PixelShader(device, L"./Shaders/ImportPShader.hlsl"));

        colorPass->AddBind(new Bind::TransformCBuffer(device, this, 0u));

        vcb = new Bind::VertexConstantBuffer<CollectibleObject::Collectible_VCB>(device, coll_vcb, 1u);
        colorPass->AddBind(vcb);

        pcb = new Bind::PixelConstantBuffer<CollectibleObject::Collectible_PCB>(device, coll_pcb, 2u);
        colorPass->AddBind(pcb);

        D3D11_RASTERIZER_DESC rastDesc = CD3D11_RASTERIZER_DESC(CD3D11_DEFAULT{});
        rastDesc.CullMode = D3D11_CULL_BACK;
        rastDesc.FillMode = D3D11_FILL_SOLID;
        colorPass->AddBind(new Bind::Rasterizer(device, rastDesc));

        techniques.insert({ "MainColorPass", colorPass });
    }


    //vertexShaderFilePath = L"./Shaders/PlaneShader.hlsl";
    //pixelShaderFilePath = L"./Shaders/PlaneShader.hlsl";
    //this->textures.push_back(Texture(device, "models\\Textures\\basketballskin.dds", aiTextureType_DIFFUSE));
}

CollectibleObject::CollectibleObject(ID3D11Device* device, const std::string& modelsFolder, const DirectX::XMFLOAT3& position)
{
    this->device = device;
    LoadRandomModel(modelsFolder);
    /*worldMat = XMMatrixScaling(0.5f, 0.5f, 0.5f) *
        XMMatrixTranslation(
            rand() % 20 - 10.0f,
            radius,
            rand() % 20 - 10.0f
        );*/

    

    appliedScale = radius / modelRadius;
    initialPosition = position;
    initialPosition.y = radius;
    // std::cout << initialPosition.x << " "  << initialPosition.y << " " << initialPosition.z << "\n";
    
    worldMat = Matrix::CreateScale(appliedScale) * Matrix::CreateTranslation(initialPosition);
}

void CollectibleObject::LoadRandomModel(const std::string& folder)
{
    static std::random_device rd;
    static std::mt19937 gen(rd());

    auto models = GetModelList(folder);
    if (models.empty()) return;

    std::uniform_int_distribution<> distr(0, models.size() - 1);
    auto chosen_model = models[distr(gen)];
    auto model_name = StringHelper::GetFileNameWithoutExtension(chosen_model);
    ModelLoader::LoadModel(chosen_model, this, ModelLoader::VertexAttrFlags::POSITION | ModelLoader::VertexAttrFlags::TEXTURE
    | ModelLoader::VertexAttrFlags::NORMAL );
    
    // ModelLoader::LoadModel("models\\suzanne.obj", this);

    // ������� bounding radius
    float maxDistance = 0.0f;
    for (int i = 0; i < this->verticesNum; i++)
    {
        XMFLOAT3 pos = this->vertices[i].pos;
        float dist = sqrt(pos.x * pos.x + pos.y * pos.y + pos.z * pos.z);
        maxDistance = max(maxDistance, dist);
    }
    modelRadius = maxDistance;


    {
        RenderTechnique* shadowPass = new RenderTechnique("DL_ShadowMapPass");
        shadowPass->AddBind(new Bind::Topology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
        shadowPass->AddBind(new Bind::VertexBuffer(device, vertices, verticesNum, sizeof(CommonVertex)));
        shadowPass->AddBind(new Bind::IndexBuffer(device, indices, indicesNum));
        shadowPass->AddBind(new Bind::TransformCBuffer(device, this, 1u));

        techniques.insert({ "DL_ShadowMapPass", shadowPass });
    }
    {
        RenderTechnique* colorPass = new RenderTechnique("MainColorPass");

        colorPass->AddBind(new Bind::Topology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST));
        vertexShaderB = new Bind::VertexShader(device, L"./Shaders/ImportVShader.hlsl");
        colorPass->AddBind(vertexShaderB);

        numInputElements = 4;
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
        IALayoutInputElements[1] =
            D3D11_INPUT_ELEMENT_DESC{
                "COLOR",
                0,
                DXGI_FORMAT_R32G32B32A32_FLOAT,
                0,
                D3D11_APPEND_ALIGNED_ELEMENT,
                D3D11_INPUT_PER_VERTEX_DATA,
                0 };
        IALayoutInputElements[2] =
            D3D11_INPUT_ELEMENT_DESC{
                "TEXCOORD",
                0,
                DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT,
                0,
                D3D11_APPEND_ALIGNED_ELEMENT,
                D3D11_INPUT_PER_VERTEX_DATA,
                0 };
        IALayoutInputElements[3] =
            D3D11_INPUT_ELEMENT_DESC{
                "NORMAL",
                0,
                DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT,
                0,
                D3D11_APPEND_ALIGNED_ELEMENT,
                D3D11_INPUT_PER_VERTEX_DATA,
                0 };

        //    vertexShaderFilePath = L"./Shaders/ImportVShader.hlsl";
        //    pixelShaderFilePath = L"./Shaders/ImportPShader.hlsl";
        //    this->textures.push_back(Texture(device, "models\\Textures\\" + model_name + "_Diffuse.dds", aiTextureType_DIFFUSE));
        //    hasTexture = true;

        colorPass->AddBind(new Bind::InputLayout(device, IALayoutInputElements, numInputElements, vertexShaderB->GetBytecode()));
        colorPass->AddBind(new Bind::PixelShader(device, L"./Shaders/ImportPShader.hlsl"));

        D3D11_RASTERIZER_DESC rastDesc = CD3D11_RASTERIZER_DESC(CD3D11_DEFAULT{});
        rastDesc.CullMode = D3D11_CULL_BACK;
        rastDesc.FillMode = D3D11_FILL_SOLID;
        colorPass->AddBind(new Bind::Rasterizer(device, rastDesc));

        colorPass->AddBind(new Bind::VertexBuffer(device, vertices, verticesNum, sizeof(CommonVertex)));
        colorPass->AddBind(new Bind::IndexBuffer(device, indices, indicesNum));
        colorPass->AddBind(new Bind::TextureB(device, "models\\Textures\\" + model_name + "_Diffuse.dds", aiTextureType_DIFFUSE, 1u));

        colorPass->AddBind(new Bind::TransformCBuffer(device, this, 0u));

        vcb = new Bind::VertexConstantBuffer<CollectibleObject::Collectible_VCB>(device, coll_vcb, 1u);
        colorPass->AddBind(vcb);

        pcb = new Bind::PixelConstantBuffer<CollectibleObject::Collectible_PCB>(device, coll_pcb, 2u);
        colorPass->AddBind(pcb);

        techniques.insert({ "MainColorPass", colorPass });
    }

}

std::vector<std::string> CollectibleObject::GetModelList(const std::string& modelsFolder)
{
    std::vector<std::string> models;
    for (const auto& entry : std::filesystem::directory_iterator(modelsFolder))
    {
        if (entry.is_regular_file() &&
            (entry.path().extension() == ".fbx" ||
                entry.path().extension() == ".obj"))
        {
            models.push_back(entry.path().string());
        }
    }
    return models;
}

Vector3 CollectibleObject::GetCenterLocation()
{
    return Vector3(initialPosition);
}

bool CollectibleObject::CheckCollision(StickyBall* ball)
{
    if (isAttached) return false;

    Vector3 ballPos = ball->GetCenterLocation();
    Vector3 objPos = GetCenterLocation();
    float distance = Vector3::Distance(ballPos, objPos);
    return distance < (ball->radius + radius);
}

void CollectibleObject::AttachToBall(StickyBall* ball)
{
    isAttached = true;
    attachedBall = ball;

    attachTransform = worldMat * XMMatrixInverse(nullptr, Matrix::CreateScale(1/ball->radius) * ball->worldMat);
}

void CollectibleObject::Update(float deltaTime)
{
    if (isAttached && attachedBall)
    {
        worldMat = attachTransform * Matrix::CreateScale(1 / attachedBall->radius) * attachedBall->worldMat;
    }
    else {
        worldMat = Matrix::CreateScale(appliedScale) * Matrix::CreateTranslation(initialPosition);
    }
}