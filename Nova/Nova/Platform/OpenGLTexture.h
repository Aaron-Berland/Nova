#pragma once
#ifndef OPENGLTEXTURE_H
#define OPENGLTEXTURE_H
#include <Nova/Renderer/Texture.h>


namespace Nova
{
	class OpenGLTexture2D : public Texture2D
	{
	public:
		OpenGLTexture2D(const std::string &path);
		virtual ~OpenGLTexture2D();

		virtual unsigned int GetWidth() const override { return m_width; }
		virtual unsigned int GetHeight() const override { return m_height; }

		virtual void Bind(unsigned int slot = 0) const override;


	private:
		std::string m_path;
		unsigned int m_width;
		unsigned int m_height;
		unsigned int m_textureID;
	};
}

#endif