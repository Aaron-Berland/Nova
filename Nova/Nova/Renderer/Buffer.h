#pragma once
#ifndef BUFFER_H
#define BUFFER_H
#include <vector>
#include <string>
#include <initializer_list>
#include <Nova/Core.h>
#include <Nova/Log.h>
namespace Nova
{
	enum class BufferType
	{
		StaticDraw,
		DynamicDraw,
		StreamDraw
	};

	enum class ShaderDataType
	{
		None = 0,
		Float,
		Float2,
		Float3,
		Float4,
		Int,
		Int2,
		Int3,
		Int4,
		Mat3,
		Mat4,
		Bool
	};


	
	struct BufferElement
	{
		std::string name;
		ShaderDataType type;
		unsigned int location;
		unsigned int size;
		unsigned int offset;
		bool normalized;
		
		BufferElement(ShaderDataType atype = ShaderDataType::None, const std::string &aname = std::string(), unsigned int alocation = 0, bool isnormalized = false);
		inline unsigned int BufferElement::GetComponentCount() const
		{
			switch (type)
			{
			case ShaderDataType::Float:   return 1;
			case ShaderDataType::Float2:  return 2;
			case ShaderDataType::Float3:  return 3;
			case ShaderDataType::Float4:  return 4;
			case ShaderDataType::Mat3:    return 3 * 3;
			case ShaderDataType::Mat4:    return 4 * 4;
			case ShaderDataType::Int:     return 1;
			case ShaderDataType::Int2:    return 2;
			case ShaderDataType::Int3:    return 3;
			case ShaderDataType::Int4:    return 4;
			case ShaderDataType::Bool:    return 1;
			}

			NOVA_CORE_ASSERT(false, "Unknown ShaderDataType!");
			return 0;
		}



	};
	

	class BufferLayout
	{
	public:
		BufferLayout(const std::initializer_list<BufferElement> &list) : m_elements(list)
		{
			CalculateOffsetsAndStride();
		}
		inline unsigned int GetStride() const { return m_stride; }

		std::vector<BufferElement>::iterator begin() { return m_elements.begin(); }
		std::vector<BufferElement>::iterator end() { return m_elements.end(); }
		std::vector<BufferElement>::const_iterator begin() const { return m_elements.cbegin(); }
		std::vector<BufferElement>::const_iterator  end() const { return m_elements.cend(); }
	private:
		inline void CalculateOffsetsAndStride()
		{
			unsigned int tempOffset = 0;
			for (auto &element : m_elements)
			{
				element.offset = tempOffset;
				tempOffset += element.size;
			}
			m_stride = tempOffset;
		}
		std::vector<BufferElement> m_elements;
		unsigned int m_stride;
	};

	class VertexBuffer
	{
	public:
		virtual ~VertexBuffer() = default;
		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;
		virtual void UpdateData(const void* data, unsigned int size) = 0;
		virtual void UpdateSubData(const void* data, unsigned int size, unsigned int offset) = 0;
		virtual const BufferLayout& GetLayout() const = 0;
		static Ref<VertexBuffer> Create(const void* data, unsigned int size,const BufferLayout &layout, BufferType type = BufferType::StaticDraw);
		
	};
	class IndexBuffer
	{
	public:
		virtual ~IndexBuffer() = default;
		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;
		virtual unsigned int GetCount() const = 0;
		virtual void UpdateData(const unsigned int* data, unsigned int count) = 0;
		virtual void UpdateSubData(const unsigned int* data, unsigned int count, unsigned int indexOffset) = 0;
		static Ref<IndexBuffer> Create(const unsigned int* data, unsigned int count, BufferType type = BufferType::StaticDraw);
	};
}











#endif