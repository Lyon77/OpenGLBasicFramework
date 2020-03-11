#pragma once
#pragma once
#include "Tests/Test.h"

#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "Texture.h"
#include "FrameBuffer.h"
#include "CubeMap.h"
#include "Camera.h"

#include <memory>

namespace test {

	class TestPBR : public Test {
	public:
		TestPBR();
		~TestPBR();

		void OnUpdate(float deltaTime) override;
		void OnRender() override;
		void OnImGuiRender() override;
		void ProcessInput(GLFWwindow* window, float deltaTime) override;
		void MouseCallback(GLFWwindow* window, double xpos, double ypos) override;
		void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset) override;
	private:
		glm::vec3 AlbedoType();

		std::unique_ptr <VertexArray> m_VAO;
		std::unique_ptr <VertexBuffer> m_VertexBuffer;
		std::unique_ptr <IndexBuffer> m_IndexBuffer;
		std::unique_ptr <VertexArray> m_CubeVAO;
		std::unique_ptr <VertexBuffer> m_CubeVertexBuffer;
		std::unique_ptr <IndexBuffer> m_CubeIndexBuffer;
		std::unique_ptr <Shader> m_Shader;
		std::unique_ptr <Shader> m_LampShader;
		std::unique_ptr <Shader> m_SkyboxShader;
		std::unique_ptr <Shader> m_EquiretangularShader;

		std::unique_ptr <CubeMap> m_CubeMap;

		std::unique_ptr <FrameBuffer> m_FBO;

		//Textures
		std::unique_ptr <Texture> m_AlbedoTexture;
		std::unique_ptr <Texture> m_MetallicTexture;
		std::unique_ptr <Texture> m_NormalTexture;
		std::unique_ptr <Texture> m_RoughnessTexture;
		std::unique_ptr <Texture> m_EquirectangularTexture;

		glm::mat4 m_Proj, m_View;

		//Camera Information
		std::unique_ptr <Camera> m_Camera;
		float m_Speed;
		float m_FOV;
		glm::vec2 m_YawPitch;

		//Mouse Calculations
		float lastX = 960.0f / 2.0f, lastY = 540.0f / 2.0f;
		bool firstMouse = true;

		bool m_TexturedSphere;
		//Albedo Color
		std::string m_Albedo;

		//location of the lamp light
		glm::vec3 m_LampPos;

	};
}