#pragma once

#include <GL\glew.h>
#include <GLFW\glfw3.h>

// A Singleton Class
class Window
{
public:
	static Window& GetInstance(unsigned int width = 960, unsigned int height = 540);

	GLFWwindow* GetWindow() { return m_Window; }
	unsigned int GetWidth() { return m_Width; }
	unsigned int GetHeight() { return m_Height; }

private:
	Window(unsigned int width, unsigned int height);
	
	unsigned int m_Width, m_Height;
	GLFWwindow* m_Window;

	int CreateWindow(unsigned int width, unsigned int height);
};