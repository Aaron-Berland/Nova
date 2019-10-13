#include "OpenGLVertexArray.h"

#include <Nova/Core.h>
#include <Nova/Log.h>
#include <glad/glad.h>


namespace Nova
{

	static GLenum ShaderDataTypeToOpenGLBaseType(ShaderDataType type)
	{
		switch (type)
		{
		case ShaderDataType::Float:    return GL_FLOAT;
		case ShaderDataType::Float2:   return GL_FLOAT;
		case ShaderDataType::Float3:   return GL_FLOAT;
		case ShaderDataType::Float4:   return GL_FLOAT;
		case ShaderDataType::Mat3:     return GL_FLOAT;
		case ShaderDataType::Mat4:     return GL_FLOAT;
		case ShaderDataType::Int:      return GL_INT;
		case ShaderDataType::Int2:     return GL_INT;
		case ShaderDataType::Int3:     return GL_INT;
		case ShaderDataType::Int4:     return GL_INT;
		case ShaderDataType::Bool:     return GL_BOOL;
		}

		NOVA_CORE_ASSERT(false, "Unknown ShaderDataType!");
		return 0;
	}
	OpenGLVertexArray::OpenGLVertexArray() 
	{
		glCreateVertexArrays(1, &m_arrayID);
	}

	OpenGLVertexArray::~OpenGLVertexArray()
	{
		glDeleteVertexArrays(1, &m_arrayID);
	}

	void OpenGLVertexArray::Bind() const
	{
		glBindVertexArray(m_arrayID);
	}

	void OpenGLVertexArray::Unbind() const
	{
		glBindVertexArray(0);
	}

	void OpenGLVertexArray::AddVertexBuffer(const VertexBuffer &buffer)
	{
		glBindVertexArray(m_arrayID);
		buffer.Bind();
		const auto& layout = buffer.GetLayout();
		for (const auto& element : layout)
		{
			glEnableVertexAttribArray(element.location);
			glVertexAttribPointer(	element.location,
									element.GetComponentCount(),
									ShaderDataTypeToOpenGLBaseType(element.type),
									element.normalized ? GL_TRUE : GL_FALSE,
									layout.GetStride(),
									(const void*) (uintptr_t)element.offset);


		}

	}

	void OpenGLVertexArray::SetIndexBuffer(const Ref<IndexBuffer> &ibuffer)
	{
		m_indexBuffer = ibuffer;
		glBindVertexArray(m_arrayID);
		m_indexBuffer->Bind();
		/*
			TODO(BYRON): Unbind may be unnecessary
		*/
		glBindVertexArray(0);
	}

}