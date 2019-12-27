#include "Camera.h"

Camera::Camera(glm::vec3 pos, glm::vec3 front, glm::vec3 up)
	: m_Pos(pos), m_Front(front), m_Up(up), viewMatrix(glm::lookAt(pos, pos + front, up))
{
}

void Camera::Forward(float speed)
{
	m_Pos += speed * m_Front;
	UpdateViewMatrix();
}

void Camera::BackWard(float speed)
{
	m_Pos -= speed * m_Front;
	UpdateViewMatrix();
}

void Camera::Left(float speed)
{
	m_Pos -= glm::normalize(glm::cross(m_Front, m_Up)) * speed;
	UpdateViewMatrix();
}

void Camera::Right(float speed)
{
	m_Pos += glm::normalize(glm::cross(m_Front, m_Up)) * speed;
	UpdateViewMatrix();
}

void Camera::Up(float speed)
{
	glm::vec3 rightAxis = glm::normalize(glm::cross(m_Front, m_Up));
	m_Pos += glm::normalize(glm::cross(rightAxis, m_Front)) * speed;
	UpdateViewMatrix();
}

void Camera::Down(float speed)
{
	glm::vec3 rightAxis = glm::normalize(glm::cross(m_Front, m_Up));
	m_Pos -= glm::normalize(glm::cross(rightAxis, m_Front)) * speed;
	UpdateViewMatrix();
}

void Camera::UpdateYawPitch(float xOffset, float yOffset)
{
	yaw += xOffset;
	pitch += yOffset;

	if (pitch > 89.0f)
		pitch = 89.0f;
	if (pitch < -89.0f)
		pitch = -89.0f;

	glm::vec3 front;
	front.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
	front.y = sin(glm::radians(pitch));
	front.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));
	m_Front = glm::normalize(front);

	UpdateViewMatrix();
}

void Camera::SetYawPitch(float yaw, float pitch)
{
	glm::vec3 front;
	front.x = cos(glm::radians(pitch)) * cos(glm::radians(yaw));
	front.y = sin(glm::radians(pitch));
	front.z = cos(glm::radians(pitch)) * sin(glm::radians(yaw));
	m_Front = glm::normalize(front);

	UpdateViewMatrix();
}

void Camera::UpdateViewMatrix()
{
	viewMatrix = glm::lookAt(m_Pos, m_Pos + m_Front, m_Up);
}
