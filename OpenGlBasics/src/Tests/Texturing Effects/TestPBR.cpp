#include "TestPBR.h"

#include <iostream>

#include "Renderer.h"
#include "imgui/imgui.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

namespace test {

	TestPBR::TestPBR()
		: m_Proj(glm::perspective(glm::radians(45.0f), 960.0f / 540.0f, 0.1f, 100.0f)), m_View(glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, -3.0))), //glm::perspective(glm::radians(45.0f), 960.0f / 540.0f, 0.1f, 500.0f)
		m_FOV(45.0f), m_YawPitch(glm::vec2(0.0f, 0.0f)), m_Speed(10.0f),
		m_TexturedSphere(false), m_Albedo("Plastic"),
		m_LampPos(glm::vec3(0.2f, 0.3f, 0.5f))
	{
		// -------------------Create Sphere------------------

		std::vector<glm::vec3> positions;
		std::vector<glm::vec2> uv;
		std::vector<glm::vec3> normals;
		std::vector<unsigned int> indices;

		const unsigned int X_SEGMENTS = 64;
		const unsigned int Y_SEGMENTS = 64;
		const float PI = 3.14159265359;

		for (unsigned int y = 0; y <= Y_SEGMENTS; ++y)
		{
			for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
			{
				float xSegment = (float)x / (float)X_SEGMENTS;
				float ySegment = (float)y / (float)Y_SEGMENTS;

				float xPos = std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
				float yPos = std::cos(ySegment * PI);
				float zPos = std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);

				positions.push_back(glm::vec3(xPos, yPos, zPos));
				uv.push_back(glm::vec2(xSegment, ySegment));
				normals.push_back(glm::vec3(xPos, yPos, zPos));
			}
		}

		bool oddRow = false;
		for (unsigned int y = 0; y <= Y_SEGMENTS; ++y)
		{
			for (int x = 0; x <= X_SEGMENTS; ++x)
			{
				if (!oddRow)
				{
					indices.push_back(y * (X_SEGMENTS + 1) + x);
					indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
				}
				else
				{
					indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
					indices.push_back(y * (X_SEGMENTS + 1) + x);
				}
			}
			oddRow = !oddRow;
		}

		std::vector<float> data;
		for (int i = 0; i < positions.size(); ++i)
		{
			data.push_back(positions[i].x);
			data.push_back(positions[i].y);
			data.push_back(positions[i].z);
			if (uv.size() > 0)
			{
				data.push_back(uv[i].x);
				data.push_back(uv[i].y);
			}
			if (normals.size() > 0)
			{
				data.push_back(normals[i].x);
				data.push_back(normals[i].y);
				data.push_back(normals[i].z);
			}
		}

		// --------------------------------------------------

		// Properly filter across cubemap faces
		GLCall(glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS));

		GLCall(glDisable(GL_CULL_FACE));

		//Depth Testing
		GLCall(glEnable(GL_DEPTH_TEST));

		//Blending for transparent bits in the texture
		GLCall(glEnable(GL_BLEND));
		GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

		m_VAO = std::make_unique<VertexArray>();

		m_VertexBuffer = std::make_unique<VertexBuffer>(&data[0], data.size() * sizeof(float));

		//set vertex buffer to array
		VertexBufferLayout layout;
		layout.Push<float>(3);
		layout.Push<float>(2);
		layout.Push<float>(3);
		m_VAO->AddBuffer(*m_VertexBuffer, layout);

		m_IndexBuffer = std::make_unique<IndexBuffer>(&indices[0], indices.size());

		//create Vertex and Fragment source
		m_Shader = std::make_unique<Shader>("res/shaders/PBR.shader");
		m_Shader->Bind();
		m_Shader->SetUniform1i("u_AlbedoMap", 0);
		m_Shader->SetUniform1i("u_MetallicMap", 1);
		m_Shader->SetUniform1i("u_NormalMap", 2);
		m_Shader->SetUniform1i("u_RoughnessMap", 3);
		m_Shader->SetUniform1i("u_IrradianceMap", 4);
		m_Shader->SetUniform1i("u_PrefilterMap", 5);
		m_Shader->SetUniform1i("u_BRDFLUT", 6);
		m_Shader->UnBind();

		m_LampShader = std::make_unique<Shader>("res/shaders/Lamp.shader");

		m_SkyboxShader = std::make_unique<Shader>("res/shaders/CubeMap.shader");
		m_SkyboxShader->Bind();
		m_SkyboxShader->SetUniform1i("u_SkyBox", 0);
		m_SkyboxShader->SetUniform1i("u_GammaCorrect", 0);
		m_SkyboxShader->UnBind();

		//Set Textures
		m_AlbedoTexture = std::make_unique<Texture>("res/textures/rusted_iron/albedo.png");
		m_MetallicTexture = std::make_unique<Texture>("res/textures/rusted_iron/metallic.png");
		m_NormalTexture = std::make_unique<Texture>("res/textures/rusted_iron/normal.png");
		m_RoughnessTexture = std::make_unique<Texture>("res/textures/rusted_iron/roughness.png");

		// ----------------CubeMap Generation------------------
		// setup framebuffer
		m_FBO = std::make_unique<FrameBuffer>();
		m_FBO->AddRenderBufferAttachment(0, 512, 512);

		// load the HDR environment map
		m_EquirectangularTexture = std::make_unique<Texture>("res/textures/equirectangular/Arches_E_PineTree.hdr", 0);

		// Initalize the Cube Here
		float cubePositions[] = {
			// positions 

			//Back
			-1.0f,  1.0f, -1.0f,
			-1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f, -1.0f,
			 1.0f,  1.0f, -1.0f,

			 //Left
			-1.0f, -1.0f,  1.0f,
			-1.0f, -1.0f, -1.0f,
			-1.0f,  1.0f, -1.0f,
			-1.0f,  1.0f,  1.0f,

			//Right
			 1.0f, -1.0f, -1.0f,
			 1.0f, -1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f, -1.0f,

			//Front
			-1.0f, -1.0f,  1.0f,
			-1.0f,  1.0f,  1.0f,
			 1.0f,  1.0f,  1.0f,
			 1.0f, -1.0f,  1.0f,

			//Top
			-1.0f,  1.0f, -1.0f,
			 1.0f,  1.0f, -1.0f,
			 1.0f,  1.0f,  1.0f,
			-1.0f,  1.0f,  1.0f,

			//Bottom
			-1.0f, -1.0f, -1.0f,
			-1.0f, -1.0f,  1.0f,
			 1.0f, -1.0f,  1.0f,
			 1.0f, -1.0f, -1.0f
		};

		unsigned int cubeIndices[] = {
			0, 2, 1,
			2, 0, 3,

			4, 5, 6,
			6, 7, 4,

			8, 10, 9,
			10, 8, 11,

			12, 13, 14,
			14, 15, 12,

			16, 17, 18,
			18, 19, 16,

			20, 22, 21,
			22, 20, 23
		};

		m_CubeVAO = std::make_unique<VertexArray>();

		m_CubeVertexBuffer = std::make_unique<VertexBuffer>(cubePositions, 3 * 4 * 6 * sizeof(float)); //3 values, 4, points, 6 faces

		VertexBufferLayout cubeLayout;
		cubeLayout.Push<float>(3);
		m_CubeVAO->AddBuffer(*m_CubeVertexBuffer, cubeLayout);

		m_CubeIndexBuffer = std::make_unique<IndexBuffer>(cubeIndices, 6 * 6);

		m_EquiretangularShader = std::make_unique<Shader>("res/shaders/UnitCube.shader");
		m_IrradianceShader = std::make_unique<Shader>("res/shaders/Irradiance.shader");
		m_PrefilterShader = std::make_unique<Shader>("res/shaders/Prefilter.shader");
		m_BRDFShader = std::make_unique<Shader>("res/shaders/BRDF.shader");

		glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
		glm::mat4 captureViews[] =
		{
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
			glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
		};

		m_CubeMap = std::make_unique<CubeMap>(1, 512, 512);

		// convert HDR equirectangular environment map to cubemap equivalent
		m_FBO->Bind();
		m_EquiretangularShader->Bind();
		m_EquiretangularShader->SetUniform1i("u_EquirectangularMap", 0);
		m_EquirectangularTexture->Bind();

		Renderer renderer;
		glViewport(0, 0, 512, 512);
		for (unsigned int i = 0; i < 6; ++i)
		{
			glm::mat4 vp = captureProjection * captureViews[i];
			m_EquiretangularShader->SetUniformMat4f("u_VP", vp);
			//m_FBO->RenderToCubeMapFace(m_CubeMap->GetID(), i);
			m_FBO->RenderToCubeMapFace(m_CubeMap->GetID(), i);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			renderer.Draw(*m_CubeVAO, *m_CubeIndexBuffer, *m_EquiretangularShader);
		}
		m_FBO->UnBind();

		// then let OpenGL generate mipmaps from first mip face (combatting visible dots artifact)
		glBindTexture(GL_TEXTURE_CUBE_MAP, m_CubeMap->GetID());
		glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

		m_FBO->Bind();
		// Draw to Irradiance CubeMap
		m_IrradianceMap = std::make_unique<CubeMap>(1, 32, 32);

		m_FBO->ChangeRenderBufferStorage(32, 32);

		m_IrradianceShader->Bind();
		m_IrradianceShader->SetUniform1i("u_EnviromentMap", 0);
		m_CubeMap->Bind();

		glViewport(0, 0, 32, 32);
		for (unsigned int i = 0; i < 6; ++i)
		{
			glm::mat4 vp = captureProjection * captureViews[i];
			m_IrradianceShader->SetUniformMat4f("u_VP", vp);
			m_FBO->RenderToCubeMapFace(m_IrradianceMap->GetID(), i);
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

			renderer.Draw(*m_CubeVAO, *m_CubeIndexBuffer, *m_IrradianceShader);
		}

		// Create the Mipmap CubeMap
		m_PrefilterMap = std::make_unique<CubeMap>(2, 128, 128);

		m_PrefilterShader->Bind();
		m_PrefilterShader->SetUniform1i("u_EnviromentMap", 0);
		m_CubeMap->Bind();

		unsigned int maxMipLevels = 5;
		for (unsigned int mip = 0; mip < maxMipLevels; ++mip)
		{
			// reisze framebuffer according to mip-level size.
			unsigned int mipWidth = 128 * std::pow(0.5, mip);
			unsigned int mipHeight = 128 * std::pow(0.5, mip);
			m_FBO->ChangeRenderBufferStorage(mipWidth, mipHeight);
			glViewport(0, 0, mipWidth, mipHeight);

			float roughness = (float)mip / (float)(maxMipLevels - 1);
			m_PrefilterShader->SetUniform1f("u_Roughness", roughness);

			for (unsigned int i = 0; i < 6; ++i)
			{
				glm::mat4 vp = captureProjection * captureViews[i];
				m_PrefilterShader->SetUniformMat4f("u_VP", vp);
				glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, m_PrefilterMap->GetID(), mip);

				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

				renderer.Draw(*m_CubeVAO, *m_CubeIndexBuffer, *m_PrefilterShader);
			}
		}

		// Generate BRDF 2D LUT
		m_FBO->Add2DColorAttachment();
		m_FBO->Bind();
		m_FBO->ChangeRenderBufferStorage(512, 512);


		// Quad generation
		float quadPositions[] = {
			 -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
			  1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
			  1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
			 -1.0f,  1.0f, 0.0f, 0.0f, 1.0f
		};

		unsigned int quadIndices[] = {
			0, 1, 2,
			2, 3, 0
		};

		VertexArray quadVAO;

		VertexBuffer quadVertexBuffer(quadPositions, 5 * 4 * sizeof(float)); //4 values, 4, points

		VertexBufferLayout quadLayout;
		quadLayout.Push<float>(3);
		quadLayout.Push<float>(2);
		quadVAO.AddBuffer(quadVertexBuffer, quadLayout);

		IndexBuffer quadIndexBuffer(quadIndices, 6);

		glViewport(0, 0, 512, 512);
		m_BRDFShader->Bind();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		renderer.Draw(quadVAO, quadIndexBuffer, *m_BRDFShader);

		m_FBO->UnBind();
		glViewport(0, 0, 960, 540);
		// -----------------------------------------------------


		//Set Camera
		Camera::GetInstance().Reset(glm::vec3(0.0f, 0.0f, 20.0f), glm::vec3(0.0f, 0.0f, -3.0f), glm::vec3(0.0f, 1.0f, 0.0f));
	}

	TestPBR::~TestPBR()
	{
	}

	void TestPBR::OnUpdate(float deltaTime)
	{
		Camera::GetInstance().UpdateSpeed(m_Speed);
	}

	void TestPBR::OnRender()
	{
		Renderer renderer;
		GLCall(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
		GLCall(glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT));

		//Move Camera
		Camera::GetInstance().SetYawPitch(m_YawPitch.x - 90.0f, m_YawPitch.y);
		m_Proj = glm::perspective(glm::radians(m_FOV), 960.0f / 540.0f, 0.1f, 100.0f);

		// ------Make this section global----------
		int nrRows = 7;
		int nrColumns = 7;
		float spacing = 2.5;

		glm::vec3 lightPositions[] = {
			glm::vec3(-10.0f,  10.0f, 10.0f),
			glm::vec3(10.0f,  10.0f, 10.0f),
			glm::vec3(-10.0f, -10.0f, 10.0f),
			glm::vec3(10.0f, -10.0f, 10.0f)
		};
		glm::vec3 lightColors[] = {
			glm::vec3(300.0f, 300.0f, 300.0f),
			glm::vec3(300.0f, 300.0f, 300.0f),
			glm::vec3(300.0f, 300.0f, 300.0f),
			glm::vec3(300.0f, 300.0f, 300.0f)
		};
		// ----------------------------------------

		{
			m_Shader->Bind();

			//Set Uniforms
			m_Shader->SetUniform3f("u_CameraPos", Camera::GetInstance().CameraPosition().x, Camera::GetInstance().CameraPosition().y, Camera::GetInstance().CameraPosition().z);
			m_Shader->SetUniform1f("u_AO", 1.0f);
			m_Shader->SetUniform1i("u_Textured", m_TexturedSphere);

			glm::vec3 type = AlbedoType();
			m_Shader->SetUniform3f("u_Albedo", type.x, type.y, type.z);

			m_AlbedoTexture->Bind(0);
			m_MetallicTexture->Bind(1);
			m_NormalTexture->Bind(2);
			m_RoughnessTexture->Bind(3);
			m_IrradianceMap->Bind(4);
			m_PrefilterMap->Bind(5);
			m_FBO->BindColorTexture(6, 0);

			glm::mat4 model = glm::mat4(1.0f);
			m_View = Camera::GetInstance().viewMatrix;

			// Setup Lights
			for (unsigned int i = 0; i < sizeof(lightPositions) / sizeof(lightPositions[0]); ++i)
			{
				//glm::vec3 newPos = lightPositions[i] + glm::vec3(sin(glfwGetTime() * 5.0) * 5.0, 0.0, 0.0);
				//newPos = lightPositions[i];
				m_Shader->SetUniform3f("u_LightPositions[" + std::to_string(i) + "]", lightPositions[i].x, lightPositions[i].y, lightPositions[i].z);
				m_Shader->SetUniform3f("u_LightColors[" + std::to_string(i) + "]", lightColors[i].x, lightColors[i].y, lightColors[i].z);
			}

			for (int row = 0; row < nrRows; row++) 
			{
				m_Shader->SetUniform1f("u_Metallic", (float)row / (float)nrRows);

				for (int col = 0; col < nrColumns; ++col)
				{
					// we clamp the roughness to 0.025 - 1.0 as perfectly smooth surfaces (roughness of 0.0) tend to look a bit off
					// on direct lighting.
					m_Shader->SetUniform1f("u_Roughness", glm::clamp((float)col / (float)nrColumns, 0.05f, 1.0f));

					// Reset model and recalculate location
					model = glm::mat4(1.0f);
					model = glm::translate(model, glm::vec3(
						(col - (nrColumns / 2)) * spacing,
						(row - (nrRows / 2)) * spacing,
						0.0f
					));

					//construt model view projection
					glm::mat4 mvp = m_Proj * m_View * model;

					m_Shader->SetUniformMat4f("u_MVP", mvp);
					m_Shader->SetUniformMat4f("u_Model", model);

					renderer.DrawTriangleStrips(*m_VAO, *m_IndexBuffer, *m_Shader);
				}
			}

			m_Shader->UnBind();
		}

		// Load the Lamp
		{
			m_LampShader->Bind();


			for (unsigned int i = 0; i < sizeof(lightPositions) / sizeof(lightPositions[0]); ++i)
			{
				//glm::vec3 newPos = lightPositions[i] + glm::vec3(sin(glfwGetTime() * 5.0) * 5.0, 0.0, 0.0);
				//newPos = lightPositions[i];
				
				//create model matrix
				glm::mat4 model = glm::mat4(1.0f);

				model = glm::translate(model, lightPositions[i]);
				model = glm::scale(model, glm::vec3(0.1f));

				m_View = Camera::GetInstance().viewMatrix;

				//construt model view projection
				glm::mat4 mvp = m_Proj * m_View * model;

				m_LampShader->SetUniformMat4f("u_MVP", mvp);

				//draw texture
				renderer.Draw(*m_VAO, *m_IndexBuffer, *m_LampShader);
			}

			m_LampShader->UnBind();
		}

		// Draw Skybox
		{
			glDepthFunc(GL_LEQUAL);

			m_SkyboxShader->Bind();

			m_View = glm::mat4(glm::mat3(Camera::GetInstance().viewMatrix));
			glm::mat4 vp = m_Proj * m_View;
			m_SkyboxShader->SetUniformMat4f("u_VP", vp);

			//Bind CubeMap and Render it
			m_PrefilterMap->Bind();

			renderer.Draw(*m_CubeVAO, *m_CubeIndexBuffer, *m_SkyboxShader);

			m_SkyboxShader->UnBind();

			glDepthFunc(GL_LESS);
		}
	}

	void TestPBR::MouseCallback(GLFWwindow* window, double xpos, double ypos)
	{
		if (firstMouse) // this bool variable is initially set to true
		{
			lastX = xpos;
			lastY = ypos;
			firstMouse = false;
		}

		float xOffset = xpos - lastX;
		float yOffset = lastY - ypos;
		lastX = xpos;
		lastY = ypos;

		float sensitivity = 0.05f;
		xOffset *= sensitivity;
		yOffset *= sensitivity;
		Camera::GetInstance().UpdateYawPitch(xOffset, yOffset);
	}

	void TestPBR::ScrollCallback(GLFWwindow* window, double xoffset, double yoffset)
	{
		if (m_FOV >= 1.0f && m_FOV <= 45.0f)
			m_FOV -= yoffset;
		if (m_FOV <= 1.0f)
			m_FOV = 1.0f;
		if (m_FOV >= 45.0f)
			m_FOV = 45.0f;

		m_Proj = glm::perspective(glm::radians(m_FOV), 960.0f / 540.0f, 0.1f, 100.0f);
	}

	glm::vec3 TestPBR::AlbedoType()
	{
		glm::vec3 output = glm::vec3(1.0f);

		if (m_Albedo == "Plastic")
			output = glm::vec3(0.04f, 0.04f, 0.04f);
		if (m_Albedo == "Diamond")
			output = glm::vec3(0.17f, 0.17f, 0.17f);
		if (m_Albedo == "Iron")
			output = glm::vec3(0.56f, 0.57f, 0.58f);
		if (m_Albedo == "Copper")
			output = glm::vec3(0.95f, 0.64f, 0.54f);
		if (m_Albedo == "Gold")
			output = glm::vec3(1.00f, 0.71f, 0.29f);
		if (m_Albedo == "Aluminum")
			output = glm::vec3(0.91f, 0.92f, 0.92f);
		if (m_Albedo == "Silver")
			output = glm::vec3(0.95f, 0.93f, 0.88f);

		return output;
	}

	void TestPBR::OnImGuiRender()
	{
		ImGui::TextWrapped("Welcome to the PBR Test Enviroment. The metallic value ranges from 0.0 to 1.0 from top to bottom and roughness from 0.0 to 1.0 left to right. The Rusted Iron texture has all the spheres be the same. The Use WASD to move around and QE to zoom in and out so that you can see the shadows. There are more setting options below.");

		ImGui::Checkbox("Rusted Iron Texture", &m_TexturedSphere);

		std::vector<std::string> items { "Plastic", "Diamond", "Iron", "Copper", "Gold", "Aluminum", "Silver" };
		if (ImGui::BeginCombo("Albedo Color", m_Albedo.c_str())) // The second parameter is the label previewed before opening the combo.
		{
			for (int n = 0; n < items.size(); n++)
			{
				bool is_selected = (m_Albedo == items[n]);
				if (ImGui::Selectable(items[n].c_str(), is_selected))
				{
					m_Albedo = items[n];
					if (is_selected)
						ImGui::SetItemDefaultFocus();   // You may set the initial focus when opening the combo (scrolling + for keyboard navigation support)
				}
			}
			ImGui::EndCombo();
		}

		ImGui::SliderFloat2("Yaw Pitch", &m_YawPitch.x, -89.0f, 89.0f);
		ImGui::SliderFloat("FOV", &m_FOV, 1.0f, 89.0f);
		ImGui::SliderFloat("Speed", &m_Speed, 5.0f, 20.0f);

		//displays framerate
		ImGui::TextWrapped("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	}
}