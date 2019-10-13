#include "OpenGLRendererAPI.h"
#include <glad/glad.h>



namespace Nova
{
	void OpenGLRendererAPI::Init()
	{
		glEnable(GL_BLEND);
		glEnable(GL_DEPTH_TEST);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	}
	void OpenGLRendererAPI::SetClearColor(const glm::vec4 &color)
	{
		glClearColor(color.r, color.g, color.b, color.a);
	}

	void OpenGLRendererAPI::Clear()
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void OpenGLRendererAPI::DrawIndexed(const VertexArray &vao)
	{
		glDrawElements(GL_TRIANGLES, vao.GetIndexBufferCount(), GL_UNSIGNED_INT, nullptr);
	}

	void OpenGLRendererAPI::DrawArray(const VertexArray &vao)
	{
		glDrawArrays(GL_TRIANGLES, 0, vao.GetVertexCount());
	}

	void OpenGLRendererAPI::ViewportResize(unsigned int x, unsigned int y, unsigned int w, unsigned int h)
	{
		glViewport(x, y, w, h);
	}

}