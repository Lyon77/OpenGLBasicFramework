#pragma once

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

class Camera
{
public:
	Camera(glm::vec3 pos, glm::vec3 front, glm::vec3 up);
	~Camera() = default;

	void Forward(float speed);
	void BackWard(float speed);
	void Left(float speed);
	void Right(float speed);
	void Up(float speed);
	void Down(float speed);

	void UpdateYawPitch(float xOffset, float yOffset);
	void SetYawPitch(float yaw, float pitch);

	void UpdateViewMatrix();

	glm::mat4 viewMatrix;
private:
	glm::vec3 m_Pos;
	glm::vec3 m_Front;
	glm::vec3 m_Up;

	float yaw = -90.0f;
	float pitch = 0.0f;
};