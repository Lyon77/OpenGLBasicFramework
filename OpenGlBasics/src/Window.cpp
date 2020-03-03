#include "Window.h"

#include <iostream>
 

Window& Window::GetInstance(unsigned int width, unsigned int height)
{
	static Window s_Window(width, height);
	return s_Window;
}

Window::Window(unsigned int width, unsigned int height)
	: m_Width(width), m_Height(height)
{
	int success = CreateWindow(width, height);

	if (success == 0)
		std::cout << "Window Failed to Open" << std::endl;
}

int Window::CreateWindow(unsigned int width, unsigned int height)
{
	if (!glfwInit())
		return -1;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	/* Create a windowed mode window and its OpenGL context */
	m_Window = glfwCreateWindow(width, height, "Hello World", NULL, NULL);
	if (!m_Window)
	{
		glfwTerminate();
		return -1;
	}

	/* Make the window's context current */
	glfwMakeContextCurrent(m_Window);

	glfwSwapInterval(1);

	if (glewInit() != GLEW_OK)
		std::cout << "Error!" << std::endl;

	return 0;
}
