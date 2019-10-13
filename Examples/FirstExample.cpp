#include <Nova.h>
#include <imgui.h>
#include <glm/gtc/matrix_transform.hpp>
/*

TODO(BYRON): temporary
*/
#include <Nova/Platform/OpenGLShader.h>
#include <glm/gtc/type_ptr.hpp>

class ExampleLayer : public Nova::Layer
{
public:
	ExampleLayer() : Nova::Layer("Example"),m_cameraController(1280.0f/720.0f)
	{
		Nova::BufferLayout layout =
		{
			{ Nova::ShaderDataType::Float3, "a_Position", 1 },
			{ Nova::ShaderDataType::Float4, "a_Color", 0 }
		};

		Nova::BufferLayout quadtexLayout =
		{
			{Nova::ShaderDataType::Float3 , "a_Position", 0},
			{Nova::ShaderDataType::Float2 , "a_TexCoord", 1}
		};

		float vertices[3 * 7] = {
			-0.5f, -0.5f, 0.0f, 0.1f,0.8f,0.1f,1.0f,
			 0.5f, -0.5f, 0.0f, 0.1f,0.8f,0.1f,1.0f,
			 0.0f,  0.5f, 0.0f, 0.1f,0.8f,0.1f,1.0f
		};
		float squareVertices[4 * 7] =
		{
			-0.5f, -0.5f, 0.0f, 0.1f, 0.1f , 0.8f, 1.0f,
			0.5f, -0.5f, 0.0f, 0.1f, 0.1f , 0.8f,  1.0f,
			0.5f,  0.5f, 0.0f, 0.1f, 0.1f , 0.8f,  1.0f,
			-0.5f,  0.5f, 0.0f, 0.1f, 0.1f , 0.8f,  1.0f
		};

		float quadtexVertices[4 * 5] =
		{
			-0.5f, -0.5f, 0.0f, 0.0f, 0.0f,
			0.5f, -0.5f, 0.0f, 1.0f, 0.0f,
			0.5f,  0.5f, 0.0f, 1.0f, 1.0f,
			-0.5f,  0.5f, 0.0f, 0.0f, 1.0f
		};
		unsigned int indices[3] = { 0,1,2 };
		unsigned int squareIndices[6] = { 0, 1, 2, 2, 3, 0 };

		m_triangleVbo = Nova::VertexBuffer::Create(vertices, sizeof(vertices), layout);
		m_triangleIbo = Nova::IndexBuffer::Create(indices, 3);
		m_triangleVao = Nova::VertexArray::Create();
		m_triangleVao->AddVertexBuffer(*m_triangleVbo);
		m_triangleVao->SetIndexBuffer(m_triangleIbo);

		m_squareVbo = Nova::VertexBuffer::Create(squareVertices, sizeof(squareVertices), layout);
		m_squareIbo = Nova::IndexBuffer::Create(squareIndices, 6);
		m_squareVao = Nova::VertexArray::Create();
		m_squareVao->AddVertexBuffer(*m_squareVbo);
		m_squareVao->SetIndexBuffer(m_squareIbo);

		m_quadtexVbo = Nova::VertexBuffer::Create(quadtexVertices, sizeof(quadtexVertices), quadtexLayout);
		m_quadtexVao = Nova::VertexArray::Create();
		m_quadtexVao->AddVertexBuffer(*m_quadtexVbo);
		m_quadtexVao->SetIndexBuffer(m_squareIbo);


		
		m_shader = Nova::Shader::Create("Assets/Shaders/vertex.vs", "Assets/Shaders/fragment.fs");
		m_texShader = Nova::Shader::Create("Assets/Shaders/texshader.vs","Assets/Shaders/texshader.fs");
	
		std::dynamic_pointer_cast<Nova::OpenGLShader>(m_texShader)->Use();
		std::dynamic_pointer_cast<Nova::OpenGLShader>(m_texShader)->SetInt("u_Texture", 0);
	}
	void OnUpdate(Nova::Timestep ts) override
	{
		

		m_cameraController.OnUpdate(ts);
	
		Nova::RenderCommand::SetClearColor({ 0.1f,0.1f,0.1f,1.0f });
		Nova::RenderCommand::Clear();


		Nova::Renderer::BeginScene(m_cameraController.GetCamera());
		std::dynamic_pointer_cast<Nova::OpenGLShader>(m_shader)->Use();
		std::dynamic_pointer_cast<Nova::OpenGLShader>(m_shader)->SetUniform3f("u_Color", m_triangleColor);
		Nova::Renderer::Submit(*m_shader, *m_triangleVao);
		Nova::Renderer::Submit(*m_texShader, *m_quadtexVao, glm::scale(glm::mat4(1.0f), glm::vec3(1.5f)));
		Nova::Renderer::EndScene();
	}
	void OnEvent(Nova::Event &event) override
	{

		m_cameraController.OnEvent(event);
	}
	void OnImGuiRender() override
	{
		ImGui::Begin("Settings");
		ImGui::ColorEdit3("Triangle Color", glm::value_ptr(m_triangleColor));
		ImGui::ColorEdit3("Square Color", glm::value_ptr(m_squareColor));
		ImGui::End();

	}
private:

	Nova::Ref<Nova::VertexBuffer> m_triangleVbo;
	Nova::Ref<Nova::IndexBuffer> m_triangleIbo;
	Nova::Ref<Nova::VertexArray> m_triangleVao;
	Nova::Ref<Nova::VertexBuffer> m_squareVbo;
	Nova::Ref<Nova::IndexBuffer> m_squareIbo;
	Nova::Ref<Nova::VertexArray> m_squareVao;
	Nova::Ref<Nova::VertexBuffer> m_quadtexVbo;
	Nova::Ref<Nova::VertexArray> m_quadtexVao;
	Nova::Ref<Nova::Shader> m_shader;
	Nova::Ref<Nova::Shader> m_texShader;

	Nova::OrthographicCameraController m_cameraController;


	glm::vec3 m_squareColor = { 0.2f,0.3f,0.8f };
	glm::vec3 m_triangleColor = { 0.8f,0.2f,0.3f };
};
class firstApp : public Nova::Application {
public:
	firstApp() 
	{
		PushLayer(new ExampleLayer());
		//PushOverlay(new Nova::ImGuiLayer());
	}
	~firstApp() = default;


};


Nova::Application* Nova::CreateApplication()
{
	
	return new firstApp();
}
