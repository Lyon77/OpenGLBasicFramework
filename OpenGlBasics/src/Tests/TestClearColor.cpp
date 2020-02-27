#include "TestClearColor.h"

#include "Renderer.h"
#include "imgui/imgui.h"

namespace test {
	TestClearColor::TestClearColor()
		: m_ClearColor { 0.2f, 0.3f, 0.8f, 1.0f }
	{
	}
	TestClearColor::~TestClearColor()
	{
	}
	void TestClearColor::OnUpdate(float deltaTime)
	{
	}
	void TestClearColor::OnRender()
	{
		//sets the color to whatever value is stored in m_ClearColor
		GLCall(glClearColor(m_ClearColor[0], m_ClearColor[1], m_ClearColor[2], m_ClearColor[3]));
		GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT));
	}
	void TestClearColor::OnImGuiRender()
	{
		ImGui::TextWrapped("Welcome to the Color Test Enviroment. Use the options below to change the color.");
		ImGui::ColorEdit4("Clear Color", m_ClearColor);
	}
	void TestClearColor::ProcessInput(GLFWwindow* window, float deltaTime)
	{
	}
}