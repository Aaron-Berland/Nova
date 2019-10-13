#pragma once
#ifndef RENDERERAPI_H
#define RENDERERAPI_H
#include <Nova/Renderer/VertexArray.h>
#include <glm/glm.hpp>

namespace Nova
{
	

	class RendererAPI
	{
	public:
		enum class API
		{
			None = 0,
			OpenGL = 1
		};
		

		virtual void Init() = 0;
		virtual void SetClearColor(const glm::vec4  &color) = 0;
		virtual void Clear() = 0;
		virtual void DrawIndexed(const VertexArray &vao) = 0;
		virtual void DrawArray(const VertexArray &vao) = 0;
		virtual void ViewportResize(unsigned int x, unsigned int y, unsigned int w, unsigned int h) = 0;
		inline static API GetAPI() { return s_API; }

	private:
		static API s_API;
	};
}



#endif