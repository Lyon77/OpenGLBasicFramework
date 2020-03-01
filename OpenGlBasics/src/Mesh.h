#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include <functional>
#include <vector>
#include <string>
#include <memory>

#include "Shader.h"
#include "VertexArray.h"
#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "IndexBuffer.h"
#include "Renderer.h"


struct MeshVertex 
{
	glm::vec3 Position;
	glm::vec3 Normal;
	glm::vec2 TexCoords;
};

struct MeshTexture 
{
	unsigned int id;
	std::string type;
	std::string path;
};

class Mesh
{
public:
	std::vector<MeshVertex> vertices;
	std::vector<unsigned int> indices;
	std::vector<MeshTexture> textures;

	Mesh(std::vector<MeshVertex> vertices, std::vector<unsigned int> indices, std::vector<MeshTexture> textures);
	//Mesh();

	void Draw(Shader shader);
private:
	std::unique_ptr <VertexArray> m_VAO;
	std::unique_ptr <VertexBuffer> m_VertexBuffer;
	std::unique_ptr <IndexBuffer> m_IndexBuffer;

	void setupMesh();
};