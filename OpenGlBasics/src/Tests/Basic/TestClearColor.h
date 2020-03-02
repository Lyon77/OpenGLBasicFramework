#pragma once

#include "Tests/Test.h"

namespace test {

	class TestClearColor : public Test {
	public:
		TestClearColor();
		~TestClearColor();

		void OnUpdate(float deltaTime) override;
		void OnRender() override;
		void OnImGuiRender() override;
		void ProcessInput(GLFWwindow* window, float deltaTime) override;
	private:
		float m_ClearColor[4];
	};
}