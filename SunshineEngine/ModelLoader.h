#pragma once
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "SceneNode.h"
#include <filesystem>
#include <random>

class ModelLoader
{
public:
    static void LoadModel(const std::string& path, SceneNode* rootNode);
    // void ProcessNode(aiNode* node, const aiScene* scene, SceneNode* parent);

    // void ProcessMesh(aiMesh* mesh, const aiScene* scene, SceneNode* meshNode)
};