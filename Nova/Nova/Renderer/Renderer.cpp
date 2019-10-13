#include "Renderer.h"
/*
	TODO(BYRON): Temporary
*/
#include <Nova/Platform/OpenGLShader.h>
namespace Nova
{
	Scope<Renderer::SceneData> Renderer::s_sceneData = CreateScope<Renderer::SceneData>();
	void Renderer::BeginScene(const Camera &camera)
	{
		s_sceneData->viewProjectionMatrix = camera.GetViewProjectionMatrix();
	}
	void Renderer::Init()
	{
		RenderCommand::Init();
	}

	void Renderer::EndScene()
	{

	}

	void Renderer::Submit(Shader &shader, const VertexArray &vao, const glm::mat4& transform)
	{
		shader.Use();
		dynamic_cast<OpenGLShader&>(shader).SetUniformMat4("u_viewProjectionMatrix", s_sceneData->viewProjectionMatrix);
		dynamic_cast<OpenGLShader&>(shader).SetUniformMat4("u_transform", transform);
		vao.Bind();
		
		RenderCommand::DrawIndexed(vao);
	}

	void Renderer::SetShaderData(Shader &shader, const glm::mat4 &transform /*= glm::mat4(1.0f)*/)
	{
		shader.Use();
		dynamic_cast<OpenGLShader&>(shader).SetUniformMat4("u_viewProjectionMatrix", s_sceneData->viewProjectionMatrix);
		dynamic_cast<OpenGLShader&>(shader).SetUniformMat4("u_transform", transform);
	}

	void Renderer::BasicSubmit(const VertexArray &vao)
	{
		vao.Bind();
		RenderCommand::DrawIndexed(vao);
	}

	void Renderer::UnIndexedBasicSubmit(const VertexArray &vao)
	{
		vao.Bind();
		RenderCommand::DrawArray(vao);
	}

}