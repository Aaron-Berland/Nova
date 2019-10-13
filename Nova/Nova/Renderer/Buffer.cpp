#include "Buffer.h"
#include <Nova/Renderer/Renderer.h>
#include <Nova/Platform/OpenGLBuffer.h>



namespace Nova
{
	inline static unsigned int ShaderDataTypeSize(ShaderDataType type)
	{
		switch (type)
		{
			case Nova::ShaderDataType::Float:		return 4;
			case Nova::ShaderDataType::Float2:		return 4 * 2;
			case Nova::ShaderDataType::Float3:		return 4 * 3;
			case Nova::ShaderDataType::Float4:		return 4 * 4;
			case Nova::ShaderDataType::Int:			return 4;
			case Nova::ShaderDataType::Int2:		return 4 * 2;
			case Nova::ShaderDataType::Int3:		return 4 * 3;
			case Nova::ShaderDataType::Int4:		return 4 * 4;
			case Nova::ShaderDataType::Mat3:		return 3 * 3 * 4;
			case Nova::ShaderDataType::Mat4:		return 4 * 4 * 4;
			case Nova::ShaderDataType::Bool:		return 1;

		}
		NOVA_CORE_ASSERT(false, "Unknown ShaderDataType");
		return 0;
	}


	BufferElement::BufferElement(ShaderDataType atype /*= ShaderDataType::None*/, const std::string &aname /*= std::string()*/, unsigned int alocation /*= 0*/, bool isnormalized /*= false*/)
	:	type(atype), name(aname), location(alocation), size(ShaderDataTypeSize(atype)), offset(0), normalized(isnormalized)
	{

	}


	


	Ref<VertexBuffer> VertexBuffer::Create(const void* data, unsigned int size, const BufferLayout &layout, BufferType type)
	{
		switch (Renderer::GetAPI())
		{
			case Nova::RendererAPI::API::None:
			{
				NOVA_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
				return nullptr;
			}
			case Nova::RendererAPI::API::OpenGL:
				return CreateRef<OpenGLVertexBuffer>(data, size, layout, type);

		}

		NOVA_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	Ref<IndexBuffer> IndexBuffer::Create(const unsigned int* data, unsigned int count, BufferType type)
	{
		switch (Renderer::GetAPI())
		{
			case Nova::RendererAPI::API::None:
			{
				NOVA_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
				return nullptr;
			}
			case Nova::RendererAPI::API::OpenGL:
				return CreateRef<OpenGLIndexBuffer>(data, count, type);

		}

		NOVA_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}




	
	

}