#include "VertexArray.h"
#include <Nova/Renderer/Renderer.h>
#include <Nova/Platform/OpenGLVertexArray.h>
#include <Nova/Core.h>
#include <Nova/Log.h>


namespace Nova
{

	Ref<VertexArray> VertexArray::Create()
	{
		switch (Renderer::GetAPI())
		{
		case Nova::RendererAPI::API::None:
		{
			NOVA_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
			return nullptr;
		}
		case Nova::RendererAPI::API::OpenGL:
			return CreateRef<OpenGLVertexArray>();

		}

		NOVA_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

}