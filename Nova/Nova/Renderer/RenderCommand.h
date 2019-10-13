#pragma once
#ifndef RENDERCOMMAND_H
#define RENDERCOMMAND_H
#include <Nova/Core.h>
#include <Nova/Renderer/RendererAPI.h>
namespace Nova
{
	class RenderCommand
	{
	public:
		inline static void Init() 
		{
			s_rendererAPI->Init();
		}
		inline static void SetClearColor(const glm::vec4 &color)
		{
			s_rendererAPI->SetClearColor(color);
		}
		inline static void Clear()
		{
			s_rendererAPI->Clear();
		}
		inline static void DrawIndexed(const VertexArray &vao)
		{
			s_rendererAPI->DrawIndexed(vao);
		}
		inline static void ViewportResize(unsigned int x, unsigned int y, unsigned int w, unsigned int h)
		{
			s_rendererAPI->ViewportResize(x, y, w, h);
		}
		inline static void DrawArray(const VertexArray &vao)
		{
			s_rendererAPI->DrawArray(vao);
		}
	private:
		static Scope<RendererAPI> s_rendererAPI;
	};
}



#endif