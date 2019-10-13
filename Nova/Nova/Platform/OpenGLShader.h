#pragma once
#ifndef OPENGLSHADER_H
#define OPENGLSHADER_H


#include <Nova/Renderer/Shader.h>
#include <string>
#include <unordered_map>
#include <glm/glm.hpp>

namespace Nova
{
	class OpenGLShader : public Shader
	{
	public:
		OpenGLShader(const OpenGLShader& aShader) = delete;
		OpenGLShader(const std::string &vertexPath,
			const std::string &fragmentPath,
			const std::string &geometryPath = std::string(),
			const std::string &tesselationControlPath = std::string(),
			const std::string &tesselationEvaluationPath = std::string());
		virtual void Use() const override;
		void SetBool(const std::string &name, bool value);
		void SetInt(const std::string& name, int value);
		void SetFloat(const std::string& name, float value);
		void SetUniform2f(const std::string& name, const glm::vec2 &value);
		void SetUniform4f(const std::string& name, const glm::vec4 &value);
		void SetUniform3f(const std::string& name, const glm::vec3 &value);
		void SetUniform3fv(const std::string& name, unsigned int count, float* fpointer);
		void SetUniformMat4(const std::string& name, const glm::mat4 &matrix);
		virtual ~OpenGLShader();

	private:
		uint32_t m_shaderID;
		std::unordered_map<std::string, int> m_uniformLocationCache;
		void CheckCompileErrors(unsigned int shader, const std::string &shaderPath) const;
		void CheckLinkingErrors(unsigned int program) const;
		int GetUniformLocation(const std::string& name);
		void LoadCode(std::string &outCode, const std::string& shaderPath, bool mandatory) const;
		unsigned int CompileShader(const std::string &shaderCode, const std::string &shaderPath, unsigned int type) const;
		unsigned int CreateProgram(unsigned int vertex,
			unsigned int fragment,
			unsigned int geometry,
			unsigned int tesselationControl,
			unsigned int tesselationEvaluation) const;
	};

}


#endif