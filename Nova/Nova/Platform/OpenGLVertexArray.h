#pragma once
#ifndef OPENGLVERTEXARRAY_H
#define OPENGLVERTEXARRAY_H
#include <Nova/Renderer/VertexArray.h>
#include <Nova/Core.h>

namespace Nova
{
	/*
		WARNING(BYRON): Maybe add here pointers to vertexbuffers and more state information
	*/
	class OpenGLVertexArray : public VertexArray
	{
	public:
		OpenGLVertexArray();
		OpenGLVertexArray(const OpenGLVertexArray& array) = delete;
		OpenGLVertexArray& operator=(const OpenGLVertexArray& array) = delete;
		virtual ~OpenGLVertexArray();
		virtual void Bind() const override;
		virtual void Unbind() const override;
		virtual void AddVertexBuffer(const VertexBuffer &buffer) override;
		virtual void SetIndexBuffer(const Ref<IndexBuffer> &ibuffer) override;
		virtual unsigned int GetIndexBufferCount() const override { return m_indexBuffer->GetCount(); }
		virtual void SetVertexCount(unsigned int count) override { m_vertexCount = count; }
		virtual unsigned int GetVertexCount() const override { return m_vertexCount; }
	private:
		unsigned int m_arrayID;
		unsigned int m_vertexCount;
		Ref<IndexBuffer> m_indexBuffer;
	};

}
#endif