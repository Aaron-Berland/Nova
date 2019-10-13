#include "Shader.h"
#include <Nova/Renderer/Renderer.h>
#include <Nova/Platform/OpenGLShader.h>
#include <Nova/Core.h>
#include <Nova/Log.h>


namespace Nova
{

	Ref<Shader> Shader::Create(	const std::string &vertexPath, 
									const std::string &fragmentPath, 
									const std::string &geometryPath /*= std::string()*/, 
									const std::string &tesselationControlPath /*= std::string()*/, 
									const std::string &tesselationEvaluationPath /*= std::string()*/)
	{
		switch (Renderer::GetAPI())
		{
			case Nova::RendererAPI::API::None:
			{
				NOVA_CORE_ASSERT(false, "RendererAPI::None is currently not supported!");
				return nullptr;
			}
			case Nova::RendererAPI::API::OpenGL:
				return CreateRef<OpenGLShader>(vertexPath, fragmentPath, geometryPath, tesselationControlPath, tesselationEvaluationPath);

		}

		NOVA_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

}