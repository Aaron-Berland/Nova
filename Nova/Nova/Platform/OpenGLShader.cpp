#include "OpenGLShader.h"
#include <sstream>
#include <fstream>
#include <vector>
#include <Nova/Core.h>
#include <Nova/Log.h>
#include <glm/gtc/type_ptr.hpp>
#include <glad/glad.h>
namespace Nova
{
	OpenGLShader::OpenGLShader(const std::string & vertexPath,
		const std::string & fragmentPath,
		const std::string & geometryPath,
		const std::string & tesselationControlPath,
		const std::string & tesselationEvaluationPath)
	{
		std::string vertexCode;
		std::string fragmentCode;
		std::string geometryCode;
		std::string tesselationControlCode;
		std::string tesselationEvaluationCode;

		LoadCode(vertexCode, vertexPath, true);
		LoadCode(fragmentCode, fragmentPath, true);
		LoadCode(geometryCode, geometryPath, false);
		LoadCode(tesselationControlCode, tesselationControlPath, false);
		LoadCode(tesselationEvaluationCode, tesselationEvaluationPath, false);

		unsigned int vertex = CompileShader(vertexCode, vertexPath, GL_VERTEX_SHADER);
		unsigned int fragment = CompileShader(fragmentCode, fragmentPath, GL_FRAGMENT_SHADER);
		unsigned int geometry = CompileShader(geometryCode, geometryPath, GL_GEOMETRY_SHADER);
		unsigned int tesselationControl = CompileShader(tesselationControlCode, tesselationControlPath, GL_TESS_CONTROL_SHADER);
		unsigned int tesselationEvaluation = CompileShader(tesselationEvaluationCode, tesselationEvaluationPath, GL_TESS_EVALUATION_SHADER);
		m_shaderID = CreateProgram(vertex, fragment, geometry, tesselationControl, tesselationEvaluation);
	}



	void OpenGLShader::LoadCode(std::string &outCode, const std::string& shaderPath, bool mandatory) const
	{

		std::ifstream shaderFile;
		//ensure ifstream objects can throw exceptions
		shaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		try
		{
			if (mandatory)
			{
				shaderFile.open(shaderPath);
				std::stringstream shaderStream;
				shaderStream << shaderFile.rdbuf();
				shaderFile.close();
				outCode = shaderStream.str();
			}
			else
			{
				if (!shaderPath.empty())
				{
					shaderFile.open(shaderPath);
					std::stringstream shaderStream;
					shaderStream << shaderFile.rdbuf();
					shaderFile.close();
					outCode = shaderStream.str();
				}

			}

		}
		catch (std::ifstream::failure e) {
			NOVA_CORE_ERROR("{0}!!", shaderPath);
			NOVA_CORE_ASSERT(false, "File openning failure!");
		}

	}

	unsigned int OpenGLShader::CompileShader(const std::string &shaderCode, const std::string &shaderPath, unsigned int type) const
	{
		if (!shaderCode.empty())
		{
			const char* cshaderCode = shaderCode.c_str();
			unsigned int shader;
			shader = glCreateShader(type);
			glShaderSource(shader, 1, &cshaderCode, 0);
			glCompileShader(shader);
			CheckCompileErrors(shader, shaderPath);
			return shader;
		}
		return 0;

	}

	unsigned int OpenGLShader::CreateProgram(unsigned int vertex, unsigned int fragment, unsigned int geometry, unsigned int tesselationControl, unsigned int tesselationEvaluation) const
	{
		GLint program = glCreateProgram();

		glAttachShader(program, vertex);
		glAttachShader(program, fragment);
		if (geometry)
			glAttachShader(program, geometry);
		if (tesselationControl)
			glAttachShader(program, tesselationControl);
		if (tesselationEvaluation)
			glAttachShader(program, tesselationEvaluation);

		glLinkProgram(program);
		CheckLinkingErrors(program);

		glDetachShader(program, vertex);
		glDeleteShader(vertex);
		glDetachShader(program, fragment);
		glDeleteShader(fragment);
		if (geometry) 
		{
			glDetachShader(program, geometry);
			glDeleteShader(geometry);
		}
		if (tesselationControl)
		{
			glDetachShader(program, tesselationControl);
			glDeleteShader(tesselationControl);
		}
		if (tesselationEvaluation)
		{
			glDetachShader(program, tesselationEvaluation);
			glDeleteShader(tesselationEvaluation);
		}
			

		return program;
	}

	void OpenGLShader::CheckCompileErrors(unsigned int shader, const std::string &shaderPath) const
	{
		GLint success = 0;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (success == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);

			// The maxLength includes the NULL character
			std::vector<GLchar> infoLog(maxLength);
			glGetShaderInfoLog(shader, maxLength, &maxLength, &infoLog[0]);

			// We don't need the shader anymore.
			glDeleteShader(shader);

			NOVA_CORE_ERROR("{0}", infoLog.data());
			NOVA_CORE_ERROR("File Location: {0}", shaderPath);
			NOVA_CORE_ASSERT(false, "Shader compilation failure!");
		}


	}



	void OpenGLShader::CheckLinkingErrors(unsigned int program) const
	{
		GLint isLinked = 0;
		glGetProgramiv(program, GL_LINK_STATUS, (int*)&isLinked);
		if (isLinked == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

			// The maxLength includes the NULL character
			std::vector<GLchar> infoLog(maxLength);
			glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);
			glDeleteProgram(program);

			NOVA_CORE_ERROR("{0}", infoLog.data());
			NOVA_CORE_ASSERT(false, "Shader linking failure!");
			return;
		}
	}

	void OpenGLShader::Use() const
	{
		glUseProgram(m_shaderID);

	}

	OpenGLShader::~OpenGLShader()
	{
		glDeleteProgram(m_shaderID);
	}

	int OpenGLShader::GetUniformLocation(const std::string& name)
	{
		if (m_uniformLocationCache.find(name) != m_uniformLocationCache.end())
		{
			return m_uniformLocationCache[name];
		}
		int location = glGetUniformLocation(m_shaderID, name.c_str());

		m_uniformLocationCache[name] = location;

		return location;
	}

	void OpenGLShader::SetBool(const std::string &name, bool value)
	{
		glUniform1i(GetUniformLocation(name), (int)value);
	}
	void OpenGLShader::SetInt(const std::string& name, int value)
	{
		glUniform1i(GetUniformLocation(name), value);
	}
	void OpenGLShader::SetFloat(const std::string& name, float value)
	{
		glUniform1f(GetUniformLocation(name), value);
	}

	void OpenGLShader::SetUniform2f(const std::string& name, const glm::vec2 &value)
	{
		glUniform2f(GetUniformLocation(name), value.x, value.y);
	}

	void OpenGLShader::SetUniform4f(const std::string& name, const glm::vec4 &value)
	{
		glUniform4f(GetUniformLocation(name), value.x, value.y, value.z, value.w);
	}

	void OpenGLShader::SetUniform3f(const std::string& name, const glm::vec3 &value)
	{
		glUniform3f(GetUniformLocation(name), value.x, value.y , value.z);
	}

	void OpenGLShader::SetUniform3fv(const std::string& name, unsigned int count, float* fpointer)
	{
		glUniform3fv(GetUniformLocation(name), count, fpointer);
	}

	void OpenGLShader::SetUniformMat4(const std::string& name, const glm::mat4& matrix)
	{
		glUniformMatrix4fv(GetUniformLocation(name), 1, GL_FALSE, glm::value_ptr(matrix));
	}

}