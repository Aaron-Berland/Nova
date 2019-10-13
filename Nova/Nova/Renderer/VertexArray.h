#pragma once
#ifndef VERTEXARRAY_H
#define VERTEXARRAY_H
#include <Nova/Core.h>
#include <Nova/Renderer/Buffer.h>

namespace Nova
{
	class VertexArray
	{
	public:
		virtual ~VertexArray() = default;
		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;
		virtual void AddVertexBuffer(const VertexBuffer &buffer) = 0;
		virtual void SetIndexBuffer(const Ref<IndexBuffer> &ibuffer) = 0;
		virtual unsigned int GetIndexBufferCount() const = 0;
		virtual void SetVertexCount(unsigned int count) = 0;
		virtual unsigned int GetVertexCount() const = 0;
		static Ref<VertexArray> Create();

	};
	
}

#endif