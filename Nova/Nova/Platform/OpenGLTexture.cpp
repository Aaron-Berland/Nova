#include "OpenGLTexture.h"
#include <stb_image.h>
#include <Nova/Core.h>
#include <Nova/Log.h>
#include <glad/glad.h>



namespace Nova
{
	OpenGLTexture2D::OpenGLTexture2D(const std::string& path) : m_path(path)
	{
		int width, height, channels;
		stbi_set_flip_vertically_on_load(1);
		stbi_uc* data = stbi_load(path.c_str(), &width, &height, &channels, 0);
		NOVA_CORE_ASSERT(data, "Failed to load image!");
		m_width = width;
		m_height = height;
		GLenum internalFormat = 0, dataFormat = 0;
		if (channels == 4)
		{
			internalFormat = GL_RGBA8;
			dataFormat = GL_RGBA;
		}
		else if (channels == 3)
		{
			internalFormat = GL_RGB8;
			dataFormat = GL_RGB;
		}

		NOVA_CORE_ASSERT(internalFormat & dataFormat, "Format not supported!");

		glCreateTextures(GL_TEXTURE_2D, 1, &m_textureID);
		glTextureStorage2D(m_textureID, 1, internalFormat, m_width, m_height);
		glTextureParameteri(m_textureID, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(m_textureID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTextureParameteri(m_textureID, GL_TEXTURE_WRAP_R, GL_REPEAT);
		glTextureParameteri(m_textureID, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTextureSubImage2D(m_textureID, 0, 0, 0, m_width, m_height, dataFormat, GL_UNSIGNED_BYTE, data);

		stbi_image_free(data);
	}

	OpenGLTexture2D::~OpenGLTexture2D()
	{
		glDeleteTextures(1, &m_textureID);
	}

	void OpenGLTexture2D::Bind(unsigned int slot) const
	{
		glBindTextureUnit(slot, m_textureID);
	}
}

