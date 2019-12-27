#pragma once
#include "Test.h"

#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "Texture.h"
#include "Camera.h"

#include <memory>

namespace test {

	class TestLighting : public Test {
	public:
		TestLighting();
		~TestLighting();

		void OnUpdate(float deltaTime) override;
		void OnRender() override;
		void OnImGuiRender() override;
		void ProcessInput(GLFWwindow* window, float deltaTime) override;
		void MouseCallback(GLFWwindow* window, double xpos, double ypos) override;
		void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset) override;
	private:
		std::unique_ptr <VertexArray> m_VAO;
		std::unique_ptr <VertexBuffer> m_VertexBuffer;
		std::unique_ptr <IndexBuffer> m_IndexBuffer;
		std::unique_ptr <Shader> m_Shader;
		//std::unique_ptr <Texture> m_Texture;

		glm::mat4 m_Proj, m_View;
		float m_FOV;
		glm::vec2 m_YawPitch;

		//the locations of the inital translations for the images
		glm::vec3 m_TranslationA;

		std::unique_ptr <Camera> m_Camera;

		float m_Speed;

		float lastX = 960.0f / 2.0f, lastY = 540.0f / 2.0f;
		bool firstMouse = true;
	};
}