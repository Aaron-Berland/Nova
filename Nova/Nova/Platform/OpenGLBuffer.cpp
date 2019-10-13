#include "OpenGLBuffer.h"
#include <glad/glad.h>

namespace Nova
{

	unsigned int BufferTypeToOpenGLEnum(BufferType type)
	{
		switch (type)
		{
		case Nova::BufferType::StaticDraw:
			return GL_STATIC_DRAW;
		case Nova::BufferType::DynamicDraw:
			return GL_DYNAMIC_DRAW;
		case Nova::BufferType::StreamDraw:
			return GL_STREAM_DRAW;
		default:
			return GL_STATIC_DRAW;
		}
		
	}

	OpenGLVertexBuffer::OpenGLVertexBuffer(const void* data, unsigned int size, const BufferLayout &layout, BufferType type) : m_layout(layout), m_openGLDrawType(BufferTypeToOpenGLEnum(type))
	{
		glCreateBuffers(1, &m_bufferID);
		UpdateData(data, size);
	}

	OpenGLVertexBuffer::~OpenGLVertexBuffer()
	{
		glDeleteBuffers(1, &m_bufferID);
	}

	void OpenGLVertexBuffer::Bind() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, m_bufferID);
	}

	void OpenGLVertexBuffer::Unbind() const
	{
		glBindBuffer(GL_ARRAY_BUFFER, 0);
	}

	void OpenGLVertexBuffer::UpdateData(const void* data, unsigned int size)
	{
		glNamedBufferData(m_bufferID, size, data, m_openGLDrawType);
	}

	void OpenGLVertexBuffer::UpdateSubData(const void* data, unsigned int size, unsigned int offset)
	{
		glNamedBufferSubData(m_bufferID, offset, size, data);
	}



	OpenGLIndexBuffer::OpenGLIndexBuffer(const unsigned int* data, unsigned int count, BufferType type) : m_openGLDrawType(BufferTypeToOpenGLEnum(type)), m_count(count)
	{
		glCreateBuffers(1, &m_bufferID);
		UpdateData(data, m_count);
	}

	OpenGLIndexBuffer::~OpenGLIndexBuffer()
	{
		glDeleteBuffers(1, &m_bufferID);
	}

	void OpenGLIndexBuffer::Bind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_bufferID);
	}

	void OpenGLIndexBuffer::Unbind() const
	{
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
	}


	void OpenGLIndexBuffer::UpdateData(const unsigned int* data, unsigned int count)
	{ 
		m_count = count;
		glNamedBufferData(m_bufferID, count * sizeof(unsigned int), data, m_openGLDrawType);
	}

	void OpenGLIndexBuffer::UpdateSubData(const unsigned int* data, unsigned int count, unsigned int indexOffset)
	{
		m_count = count + indexOffset;
		glNamedBufferSubData(m_bufferID, indexOffset * sizeof(unsigned int), count * sizeof(unsigned int), data);
	}

}