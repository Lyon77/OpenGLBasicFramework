#pragma once
#include "Tests/Test.h"

#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "Texture.h"
#include "Camera.h"
#include "Model.h"

#include <memory>

namespace test {

	class TestNanosuitModel : public Test {
	public:
		TestNanosuitModel();
		~TestNanosuitModel();

		void OnUpdate(float deltaTime) override;
		void OnRender() override;
		void OnImGuiRender() override;
		void ProcessInput(GLFWwindow* window, float deltaTime) override;
		void MouseCallback(GLFWwindow* window, double xpos, double ypos) override;
		void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset) override;
	private:
		std::unique_ptr <Shader> m_Shader;
		std::unique_ptr<Model> m_Model;

		glm::mat4 m_Proj, m_View;
		float m_FOV;
		glm::vec2 m_YawPitch;

		std::unique_ptr <Camera> m_Camera;

		float m_Speed;

		double lastX = 960.0f / 2.0f, lastY = 540.0f / 2.0f;
		bool firstMouse = true;
	};
}