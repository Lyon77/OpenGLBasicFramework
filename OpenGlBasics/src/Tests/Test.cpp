#include "Tests/Test.h"

#include "imgui/imgui.h"

#include <iostream>

namespace test {
	TestMenu::TestMenu(Test *& currentTestPointer)
		: m_CurrentTest(currentTestPointer)
	{
	}
	void TestMenu::OnImGuiRender()
	{
		ImGui::TextWrapped("Welcome to the OpenGL Test Enviroment");

		for (auto& group : m_Groups)
		{
			// Add the buttons to the group
			if (ImGui::CollapsingHeader(group.first.c_str()))
			{
				for (int i = group.second.first; i <= group.second.second; i++) {
					if (i < m_Tests.size() && i > -1) // Check if the index is within bounds
						if (ImGui::Button(m_Tests[i].first.c_str()))
							m_CurrentTest = m_Tests[i].second();
				}
			}
		}
		
	}
}