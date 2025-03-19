// Model.cpp
#include "ModelLoader.h"

template <class T>
void ModelLoader::LoadModel(const std::string& path, SceneNode<T>* rootNode)
{
	Assimp::Importer importer;
	const aiScene* pModel = importer.ReadFile(path,
		aiProcess_Triangulate);
	//	aiProcess_Triangulate | aiProcess_JoinIdenticalVertices);
	// aiProcess_GenNormals | aiProcess_CalcTangentSpace

	if (!pModel || pModel->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !pModel->mRootNode)
	{
		rootNode = nullptr;
		return;
	}

	unsigned int meshesNum = 1; // pModel->mNumMeshes;

	rootNode->verticesNum = 0;
	rootNode->indicesNum = 0;

	for (size_t i = 0; i < meshesNum; i++)
	{
		rootNode->verticesNum += pModel->mMeshes[i]->mNumVertices;
		rootNode->indicesNum += pModel->mMeshes[i]->mNumFaces * 3;
	}

	rootNode->vertices = (T*)calloc(rootNode->verticesNum, sizeof(T));
	rootNode->indices = (int*)calloc(rootNode->indicesNum, sizeof(int));

	size_t vertexIdx = 0;
	size_t indexIdx = 0;

	static std::random_device rd;
	static std::mt19937 gen(rd());
	std::uniform_real_distribution<float> distr(0, 1);

	for (size_t meshNum = 0; meshNum < meshesNum; meshNum++)
	{
		const auto pMesh = pModel->mMeshes[meshNum];
		// Обработка вершин
		for (unsigned i = 0; i < pMesh->mNumVertices; i++)
		{
			(rootNode->vertices)[vertexIdx++] = new T(
				pMesh->mVertices[i].x,
				pMesh->mVertices[i].y,
				pMesh->mVertices[i].z);
		}
		// Обработка индексов
		for (unsigned i = 0; i < pMesh->mNumFaces; i++)
		{
			aiFace face = pMesh->mFaces[i];
			assert(face.mNumIndices == 3);
			
			// auto col = XMFLOAT4(distr(gen), distr(gen), distr(gen), 1);

			for (unsigned j = 0; j < face.mNumIndices; j++) {
				(rootNode->indices)[indexIdx++] = face.mIndices[j];
				
				//(rootNode->vertices)[face.mIndices[j]].color = col;

			}
		}
	}
	return;
}

/*
void ModelLoader::ProcessNode(aiNode* node, const aiScene* scene, SceneNode* parent)
{
	// Обработка мешей
	for (unsigned i = 0; i < node->mNumMeshes; i++)
	{
		aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
		// parent->AddChild(ProcessMesh(mesh, scene));
	}

	// Рекурсивная обработка дочерних узлов
	for (unsigned i = 0; i < node->mNumChildren; i++)
	{
		SceneNode* childNode = new SceneNode();
		parent->AddChild(childNode);
		ProcessNode(node->mChildren[i], scene, childNode);
	}
}



void ModelLoader::ProcessMesh(aiMesh* mesh, const aiScene* scene, SceneNode* meshNode)
{
	std::vector<Vertex> vertices;
	std::vector<int> indices;

	// Обработка вершин
	for (unsigned i = 0; i < mesh->mNumVertices; i++)
	{
		Vertex vertex;
		vertex.pos = XMFLOAT3(
			mesh->mVertices[i].x,
			mesh->mVertices[i].y,
			mesh->mVertices[i].z
		);
		vertices.push_back(vertex);
	}

	// Обработка индексов
	for (unsigned i = 0; i < mesh->mNumFaces; i++)
	{
		aiFace face = mesh->mFaces[i];
		for (unsigned j = 0; j < face.mNumIndices; j++)
			indices.push_back(face.mIndices[j]);
	}

	meshNode->vertices = vertices.data();
	meshNode->verticesNum = vertices.size();
	meshNode->indices = indices.data();
	meshNode->indicesNum = indices.size();

	return;
}
*/