#include "Mesh.h"

#include <iostream>

Mesh::Mesh(std::vector<MeshVertex> v, std::vector<unsigned int> i, std::vector<MeshTexture> t)
	: vertices(v), indices(i), textures(t)
{
	setupMesh();
}

void Mesh::Draw(Shader *shader)
{
	unsigned int diffuseNr = 1;
	unsigned int specularNr = 1;
	unsigned int normalNr = 1;
	unsigned int heightNr = 1;

	for (int i = 0; i < textures.size(); i++)
	{
		glActiveTexture(GL_TEXTURE0 + i);

		std::string number;
		std::string name = textures[i].type;

		if (name == "texture_diffuse")
			number = std::to_string(diffuseNr++);
		else if (name == "texture_specular")
			number = std::to_string(specularNr++);
		else if (name == "texture_normal")
			number = std::to_string(normalNr++);
		else if (name == "texture_height")
			number = std::to_string(heightNr++);

		shader->SetUniform1i((name + number).c_str(), i);
		glBindTexture(GL_TEXTURE_2D, textures[i].id);
	}

	//draw mesh
	Renderer renderer;
	renderer.Draw(*m_VAO, *m_IndexBuffer, *shader);

	glActiveTexture(GL_TEXTURE0);
}

void Mesh::setupMesh()
{
	m_VAO = std::make_unique<VertexArray>();
	m_VAO->Bind();

	m_VertexBuffer = std::make_unique<VertexBuffer>(&vertices[0], vertices.size() * sizeof(MeshVertex));

	m_IndexBuffer = std::make_unique<IndexBuffer>(&indices[0], indices.size());

	//set vertex buffer to array
	VertexBufferLayout layout;
	layout.Push<float>(3);
	layout.Push<float>(3);
	layout.Push<float>(2);
	layout.Push<float>(3);
	layout.Push<float>(3);
	m_VAO->AddBuffer(*m_VertexBuffer, layout);

	m_VAO->UnBind();
}
