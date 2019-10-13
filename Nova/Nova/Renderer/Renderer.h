#pragma once
#ifndef RENDERER_H
#define RENDERER_H
#include <Nova/Core.h>
#include <Nova/Renderer/RenderCommand.h>
#include <Nova/Renderer/Camera.h>
#include <Nova/Renderer/Shader.h>
namespace Nova
{
	class Renderer
	{
	public:
		static void Init();
		static void BeginScene(const Camera &camera);
		static void EndScene();
		static void Submit(Shader &shader,const VertexArray &vao, const glm::mat4 &transform = glm::mat4(1.0f));
		static void SetShaderData(Shader &shader, const glm::mat4 &transform = glm::mat4(1.0f));
		static void BasicSubmit(const VertexArray &vao);
		static void UnIndexedBasicSubmit(const VertexArray &vao);

		inline static RendererAPI::API GetAPI() { return RendererAPI::GetAPI(); }
	private:
		struct SceneData
		{
			glm::mat4 viewProjectionMatrix;
		};
		static Scope<SceneData> s_sceneData;
	};
}
#endif