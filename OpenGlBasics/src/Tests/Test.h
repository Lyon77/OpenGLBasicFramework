#pragma once
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <functional>
#include <vector>
#include <string>

namespace test {

	class Test {
	public:
		Test() {}
		virtual ~Test() {}

		virtual void OnUpdate(float deltaTime) {}
		virtual void OnRender() {}
		virtual void OnImGuiRender() {}
		virtual void ProcessInput(GLFWwindow *window, float deltaTime) {}
		virtual void MouseCallback(GLFWwindow* window, double xpos, double ypos) {}
		virtual void ScrollCallback(GLFWwindow* window, double xoffset, double yoffset) {}
	};

	class TestMenu : public Test {
	public:
		TestMenu(Test*& currentTestPointer);

		void OnImGuiRender() override;

		template<typename T>
		void RegisterTest(const std::string& name) {
			std::cout << "Registering test " << name << std::endl;

			m_Tests.push_back(std::make_pair(name, []() { return new T(); }));
		}
	private:
		std::vector<std::pair<std::string, std::function<Test*()>>> m_Tests;
		Test*&  m_CurrentTest;
	};

}