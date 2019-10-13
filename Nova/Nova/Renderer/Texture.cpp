#include "Texture.h"
#include <Nova/Renderer/Renderer.h>
#include <Nova/Platform/OpenGLTexture.h>
#include <Nova/Log.h>


namespace Nova
{

	Ref<Texture2D> Texture2D::Create(const std::string& path)
	{
		switch (Renderer::GetAPI())
		{
			case RendererAPI::API::None:   NOVA_CORE_ASSERT(false, "RendererAPI::None is currently not supported!"); return nullptr;
			case RendererAPI::API::OpenGL:  return CreateRef<OpenGLTexture2D>(path);
		}

		NOVA_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

}