#pragma once
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Renderer/ResourceView.hpp"
#include <string>
#include <vector>

struct ID3D11Buffer;
class Renderer;

struct BufferConfig {
	ResourceFormat	m_format = ResourceFormat::UNKNOWN;
	size_t			m_numElements = 0;
	size_t			m_stride = 0;
	void*			m_data = nullptr;
};

class StructuredBuffer
{
	friend class Renderer; // Only the Renderer can create new Texture objects!

private:
	StructuredBuffer(); // can't instantiate directly; must ask Renderer to do it for you
	StructuredBuffer(StructuredBuffer const& copy) = delete; // No copying allowed!  This represents GPU memory.
public:
	~StructuredBuffer();
	ResourceView*		GetOrCreateResourceViewByFlag(RESOURCE_BINDFLAGS flag);

protected:
	BufferConfig		m_config;
	Renderer*			m_owner;

	ID3D11Buffer*			m_buffer = nullptr; 
	std::vector<ResourceView*>  m_views;
	//ID3D11ShaderResourceView*	m_shaderResourceView = nullptr;
};


