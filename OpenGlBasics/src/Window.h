#pragma once

#include <GL\glew.h>
#include <GLFW\glfw3.h>

// A Singleton Class
class Window
{
public:
	Window(unsigned int width, unsigned int height);
	~Window() = default;

	unsigned int GetWidth() { return m_Width; }
	unsigned int GetHeight() { return m_Height;  }

private:
	unsigned int m_Width, m_Height;

	int CreateWindow(unsigned int width, unsigned int height);
};