#include <Nova/Renderer/Buffer.h>


namespace Nova
{

	inline static unsigned int BufferTypeToOpenGLEnum(BufferType type);
	class OpenGLVertexBuffer : public VertexBuffer
	{
	public:
		OpenGLVertexBuffer(const void* data, unsigned int size, const BufferLayout &layout, BufferType type);
		OpenGLVertexBuffer(const OpenGLVertexBuffer& buffer) = delete;
		OpenGLVertexBuffer& operator=(const OpenGLVertexBuffer& buffer) = delete;
		virtual ~OpenGLVertexBuffer();
		virtual void Bind() const;
		virtual void Unbind() const;
		virtual void UpdateData(const void* data, unsigned int size);
		virtual void UpdateSubData(const void* data, unsigned int size, unsigned int offset);
		virtual const BufferLayout& GetLayout() const override { return m_layout; }
	private:
		BufferLayout m_layout;
		unsigned int m_bufferID;
		unsigned int m_openGLDrawType;
	};

	class OpenGLIndexBuffer : public IndexBuffer
	{
	public:
		OpenGLIndexBuffer(const unsigned int* data, unsigned int count, BufferType type);
		OpenGLIndexBuffer(const OpenGLIndexBuffer& buffer) = delete;
		OpenGLIndexBuffer& operator=(const OpenGLIndexBuffer& buffer) = delete;
		virtual ~OpenGLIndexBuffer();
		virtual void Bind() const;
		virtual void Unbind() const;
		virtual unsigned int GetCount() const { return m_count; }
		virtual void UpdateData(const unsigned int* data, unsigned int count);
		virtual void UpdateSubData(const unsigned int* data, unsigned int count, unsigned int indexOffset);
	private:
		unsigned int m_bufferID;
		unsigned int m_openGLDrawType;
		unsigned int m_count;
	};
}