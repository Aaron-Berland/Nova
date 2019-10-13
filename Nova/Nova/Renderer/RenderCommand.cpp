#include "RenderCommand.h"
#include <Nova/Platform/OpenGLRendererAPI.h>

namespace Nova
{
	static inline Scope<RendererAPI> CreateRendererAPI()
	{
		switch (RendererAPI::GetAPI())
		{
		case RendererAPI::API::None:
		{
			NOVA_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
			return nullptr;
		}
		case Nova::RendererAPI::API::OpenGL:
			return CreateScope<OpenGLRendererAPI>() ;

		}

		NOVA_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	Scope<RendererAPI> RenderCommand::s_rendererAPI = CreateRendererAPI();

}