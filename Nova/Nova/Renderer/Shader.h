#pragma once
#ifndef SHADER_H
#define SHADER_H
#include <string>
#include <Nova/Core.h>
namespace Nova
{
	class Shader
	{
	public:
		virtual ~Shader() = default;
		virtual void Use() const = 0;
		static Ref<Shader> Create(const std::string &vertexPath,
			const std::string &fragmentPath,
			const std::string &geometryPath = std::string(),
			const std::string &tesselationControlPath = std::string(),
			const std::string &tesselationEvaluationPath = std::string());
	};
}


#endif

