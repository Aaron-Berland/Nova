#pragma once
#ifndef OPENGLRENDERERAPI_H
#define OPENGLRENDERERAPI_H
#include <Nova/Renderer/RendererAPI.h>
namespace Nova
{
	class OpenGLRendererAPI : public RendererAPI
	{
	public:
		virtual void Init() override;
		virtual void SetClearColor(const glm::vec4 &color) override;
		virtual void Clear() override;
		virtual void DrawIndexed(const VertexArray &vao) override;
		virtual void DrawArray(const VertexArray &vao) override;
		virtual void ViewportResize(unsigned int x, unsigned int y, unsigned int w, unsigned int h) override;
	};
}

#endif