#pragma once
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <vector>
#include <string>

#include "Shader.h"
#include "Mesh.h"

class Model
{
public:
	Model(char* path);
	~Model();

	void Draw(Shader shader);
private:
	std::vector<Mesh> m_Meshes;
	std::string m_Directory;
	std::vector<MeshTexture> textures_loaded;

	void loadModel(std::string path);
	void processNode(aiNode* node, const aiScene* scene);
	Mesh processMesh(aiMesh* mesh, const aiScene* scene);
	std::vector<MeshTexture> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
};