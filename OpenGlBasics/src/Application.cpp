#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "Window.h"

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "Renderer.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"

#include "Tests/Basic/TestClearColor.h"
#include "Tests/Basic/TestTexture2D.h"
#include "Tests/Basic/TestAdvancedTextures.h"

#include "Tests/Lighting and Shadows/TestLighting.h"
#include "Tests/Lighting and Shadows/TestAdvancedLighting.h"
#include "Tests/Lighting and Shadows/TestDirShadows.h"
#include "Tests/Lighting and Shadows/TestPointSpotShadows.h"

#include "Tests/Post-Processing/TestDepthStencilBlending.h"
#include "Tests/Post-Processing/TestFrameBuffer.h"
#include "Tests/Post-Processing/TestHDR.h"
#include "Tests/Post-Processing/TestBloom.h"

#include "Tests/Texturing Effects/TestCubeMaps.h"
#include "Tests/Texturing Effects/TestNormalMaps.h"
#include "Tests/Texturing Effects/TestPBR.h"

#include "Tests/Model Loading/TestNanosuitModel.h"


int main(void)
{
	GLFWwindow* window = Window::GetInstance(960, 540).GetWindow();

	std::cout << glGetString(GL_VERSION) << std::endl;

	{

		Renderer renderer;

		//Use ImGui
		const char* glsl_version = "#version 130";
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGui::StyleColorsDark();
		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init(glsl_version);

		//create MainMenu
		test::Test* currentTest = nullptr;
		test::TestMenu* testMenu = new test::TestMenu(currentTest);
		currentTest = testMenu;

		testMenu->MakeGroup("Basic Features", 0, 2);
		testMenu->RegisterTest<test::TestClearColor>("Clear Color");
		testMenu->RegisterTest<test::TestTexture2D>("2D Texture");
		testMenu->RegisterTest<test::TestAdvancedTextures>("Advanced Texture and Camera Control");

		testMenu->MakeGroup("Lighting and Shadows", 3, 6);
		testMenu->RegisterTest<test::TestLighting>("Lighting");
		testMenu->RegisterTest<test::TestAdvancedLighting>("Advanced Lighting");
		testMenu->RegisterTest<test::TestDirShadows>("Directional Shadows");
		testMenu->RegisterTest<test::TestPointSpotShadows>("Point/Spot Shadows");

		testMenu->MakeGroup("Post-Processing", 7, 10);
		testMenu->RegisterTest<test::TestDepthStencilBlending>("Depth Testing, Stencil Testing, and Blending");
		testMenu->RegisterTest<test::TestFrameBuffer>("Post-Processing Effects");
		testMenu->RegisterTest<test::TestHDR>("HDR");
		testMenu->RegisterTest<test::TestBloom>("Bloom");
		
		testMenu->MakeGroup("Texturing Effects", 11, 13);
		testMenu->RegisterTest<test::TestCubeMaps>("Skybox and Reflective Materials");
		testMenu->RegisterTest<test::TestNormalMaps>("Normal Maps");
		testMenu->RegisterTest<test::TestPBR>("PBR");

		testMenu->MakeGroup("Model Loading", 14, 14);
		testMenu->RegisterTest<test::TestNanosuitModel>("Nanosuit");

		testMenu->MakeGroup("Example Enviroments", 15, 15);
		
		//keep track of time
		float deltaTime = 0.0f;
		float lastFrame = 0.0f;

		////Set Mouse to be invisible and within the window
		//glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

		//glfwSetCursorPosCallback(window, currentTest->MouseCallback);
		//glfwSetScrollCallback(window, currentTest->ScrollCallback);

		/* Loop until the user closes the window */
		while (!glfwWindowShouldClose(window))
		{
			//update time
			float currentFrame = (float) glfwGetTime();
			deltaTime = currentFrame - lastFrame;
			lastFrame = currentFrame;

			GLCall(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));

			// Render here 
			renderer.Clear();

			//Get new ImGui frame
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();
			if (currentTest) {
				currentTest->ProcessInput(window, deltaTime);
				currentTest->OnUpdate(0.0f);
				currentTest->OnRender();
				ImGui::Begin("Test");
				if (currentTest != testMenu && ImGui::Button("<-")) {
					delete currentTest;
					currentTest = testMenu;
				}
				currentTest->OnImGuiRender();
				ImGui::End();
			}

			//ImGui render
			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

			/* Swap front and back buffers */
			glfwSwapBuffers(window);

			/* Poll for and process events */
			glfwPollEvents();
		}

		if (currentTest != testMenu)
			delete testMenu;
		delete currentTest;
		
	}

	//close ImGui
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwTerminate();
	return 0;
}