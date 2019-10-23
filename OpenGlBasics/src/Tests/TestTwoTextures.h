#pragma once

#include "Test.h"

namespace test {

	class TestTwoTextures : public Test {
	public:
		TestTwoTextures();
		~TestTwoTextures();

		void OnUpdate(float deltaTime) override;
		void OnRender() override;
		void OnImGuiRender() override;
	private:
		const float positions[] = { -50.0f, -50.0f, 0.0f, 0.0f };
		const unsigned int indicies[] = { 0, 1, 2 };
	};
}