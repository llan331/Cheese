#include <Cheese.h>

#include "Platform/OpenGL/OpenGLShader.h"
#include "imgui/imgui.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class ExampleLayer : public Cheese::Layer
{
public:
	ExampleLayer()
		: Layer("Example"), m_CameraController(1280.0f / 720.0f)
	{

		/// <summary>
		/// m_VertexArray
		/// </summary>
		m_VertexArray.reset(Cheese::VertexArray::Create());

		float vertices[3 * 7] = {
			-0.5f, -0.5f, 0.0f, 0.8f, 0.2f, 0.8f, 1.0f,
			 0.5f, -0.5f, 0.0f, 0.2f, 0.3f, 0.8f, 1.0f,
			 0.0f,  0.5f, 0.0f, 0.8f, 0.8f, 0.2f, 1.0f
		};

		Cheese::Ref<Cheese::VertexBuffer> vertexBuffer;
		vertexBuffer.reset(Cheese::VertexBuffer::Create(vertices, sizeof(vertices)));
		Cheese::BufferLayout layout = {
			{Cheese::ShaderDataType::Float3, "a_Position" },
			{Cheese::ShaderDataType::Float4, "a_Color" }
		};
		vertexBuffer->SetLayout(layout);
		m_VertexArray->AddVertexBuffer(vertexBuffer);

		uint32_t indices[3] = { 0, 1, 2 };
		Cheese::Ref<Cheese::IndexBuffer> indexBuffer;
		indexBuffer.reset(Cheese::IndexBuffer::Create(indices, sizeof(indices) / sizeof(uint32_t)));
		m_VertexArray->SetIndexBuffer(indexBuffer);

		/// <summary>
		/// m_SquareVA
		/// </summary>
		m_SquareVA.reset(Cheese::VertexArray::Create());

		float squareVertices[5 * 4] = {
			-0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
			 0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
			 0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
			-0.5f,  0.5f, 0.0f, 0.0f, 1.0f
		};

		Cheese::Ref<Cheese::VertexBuffer> squareVB;
		squareVB.reset(Cheese::VertexBuffer::Create(squareVertices, sizeof(squareVertices)));
		squareVB->SetLayout({
			{Cheese::ShaderDataType::Float3, "a_Position" },
			{Cheese::ShaderDataType::Float2, "a_TexCoord" }
			});
		m_SquareVA->AddVertexBuffer(squareVB);

		uint32_t squareIndices[6] = { 0, 1, 2, 2, 3, 0 };
		Cheese::Ref<Cheese::IndexBuffer> squareIB;
		squareIB.reset(Cheese::IndexBuffer::Create(squareIndices, sizeof(squareIndices) / sizeof(uint32_t)));
		m_SquareVA->SetIndexBuffer(squareIB);

		/// <summary>
		/// m_Shader
		/// </summary>
		std::string vertexSrc = R"(
			#version 330 core
			
			layout(location = 0) in vec3 a_Position;
			layout(location = 1) in vec4 a_Color;

			uniform mat4 u_ViewProjection;
			uniform mat4 u_Transform;

			out vec3 v_Position;
			out vec4 v_Color;

			void main()
			{
				v_Position = a_Position;
				v_Color = a_Color;
				gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);	
			}
		)";

		std::string fragmentSrc = R"(
			#version 330 core
			
			layout(location = 0) out vec4 color;

			in vec3 v_Position;
			in vec4 v_Color;

			void main()
			{
				//color = vec4(v_Position * 0.5 + 0.5, 1.0);
				color = v_Color;
			}
		)";

		m_Shader = Cheese::Shader::Create("VertexPosColor", vertexSrc, fragmentSrc);

		/// <summary>
		/// m_FlatColorShader
		/// </summary>
		std::string flatColorShaderVertexSrc = R"(
			#version 330 core
			
			layout(location = 0) in vec3 a_Position;

			uniform mat4 u_ViewProjection;
			uniform mat4 u_Transform;

			out vec3 v_Position;

			void main()
			{
				v_Position = a_Position;
				gl_Position = u_ViewProjection * u_Transform * vec4(a_Position, 1.0);	
			}
		)";

		std::string flatColorShaderFragmentSrc = R"(
			#version 330 core
			
			layout(location = 0) out vec4 color;

			in vec3 v_Position;
			
			uniform vec3 u_Color;

			void main()
			{
				color = vec4(u_Color, 1.0);
			}
		)";

		m_FlatColorShader = Cheese::Shader::Create("FlatColor", flatColorShaderVertexSrc, flatColorShaderFragmentSrc);

		auto textureShader = m_ShaderLibrary.Load("assets/shaders/Texture.glsl");

		m_Texture = Cheese::Texture2D::Create("assets/textures/Checkerboard.png");
		m_ChernoLogoTexture = Cheese::Texture2D::Create("assets/textures/ChernoLogo.png");

		std::dynamic_pointer_cast<Cheese::OpenGLShader>(textureShader)->Bind();
		std::dynamic_pointer_cast<Cheese::OpenGLShader>(textureShader)->UploadUniformInt("u_Texture", 0);
	}

	void OnUpdate(Cheese::Timestep ts) override
	{
		// Update
		m_CameraController.OnUpdate(ts);

		// Render
		Cheese::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
		Cheese::RenderCommand::Clear();

		Cheese::Renderer::BeginScene(m_CameraController.GetCamera());

		glm::mat4 scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));

		std::dynamic_pointer_cast<Cheese::OpenGLShader>(m_FlatColorShader)->Bind();
		std::dynamic_pointer_cast<Cheese::OpenGLShader>(m_FlatColorShader)->UploadUniformFloat3("u_Color", m_SquareColor);

		for (int y = 0; y < 20; y++)
		{
			for (int x = 0; x < 20; x++)
			{
				glm::vec3 pos(x * 0.11f, y * 0.11f, 0.0f);
				glm::mat4 transform = glm::translate(glm::mat4(1.0f), pos) * scale;
				Cheese::Renderer::Submit(m_FlatColorShader, m_SquareVA, transform);
			}
		}

		auto textureShader = m_ShaderLibrary.Get("Texture");

		m_Texture->Bind();
		Cheese::Renderer::Submit(textureShader, m_SquareVA, glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));
		m_ChernoLogoTexture->Bind();
		Cheese::Renderer::Submit(textureShader, m_SquareVA, glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));

		Cheese::Renderer::EndScene();
	}

	virtual void OnImGuiRender() override
	{
		ImGui::Begin("Settings");
		ImGui::ColorEdit3("Square Color", glm::value_ptr(m_SquareColor));
		ImGui::End();
	}

	void OnEvent(Cheese::Event& e) override
	{
		m_CameraController.OnEvent(e);
	}
private:
	Cheese::ShaderLibrary m_ShaderLibrary;
	Cheese::Ref<Cheese::Shader> m_Shader;
	Cheese::Ref<Cheese::VertexArray> m_VertexArray;

	Cheese::Ref<Cheese::Shader> m_FlatColorShader;
	Cheese::Ref<Cheese::VertexArray> m_SquareVA;
		
	Cheese::Ref<Cheese::Texture2D> m_Texture, m_ChernoLogoTexture;

	Cheese::OrthographicCameraController m_CameraController;
	glm::vec3 m_SquareColor = { 0.2f, 0.3f, 0.8f };
};

class Sandbox : public Cheese::Application
{
public:
	Sandbox()
	{
		PushLayer(new ExampleLayer());
	}

	~Sandbox()
	{

	}

};

Cheese::Application* Cheese::CreateApplication()
{
	return new Sandbox();
}

