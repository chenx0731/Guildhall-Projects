#pragma once
#include "ThirdParty/stb/stb_image.h"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Renderer/ResourceView.hpp"
#include "Engine/Core/StringUtils.hpp"
#include <string>
#include <vector>
#define DX_SAFE_RELEASE(dxObject)	\
{									\
	if ((dxObject) != nullptr)		\
	{								\
		(dxObject)->Release();		\
		(dxObject) = nullptr;		\
	}								\
}
struct ID3D11Resource;
struct ID3D11Texture2D;
struct ID3D11Texture3D;
struct ID3D11ShaderResourceView;
struct ID3D11UnorderedAccessView;
class Renderer;

// add a function to convert from enum to DX11 enum

enum class TextureDimension
{
	TEXTURE_2D,
	TEXTURE_3D,
};

enum class TextureType
{
	DEFAULT,
	CUBEMAP
};

struct TextureConfig
{
	std::string  m_name;
	unsigned int m_width;
	unsigned int m_height;
	ResourceFormat m_format;
	unsigned int m_bindFlags;
	void*		 m_initialData = nullptr;
	int			 m_stride;
	MemoryUsage  m_usage = MemoryUsage::IMMUTABLE;
	TextureDimension m_dimension = TextureDimension::TEXTURE_2D;
	TextureType  m_type = TextureType::DEFAULT;
	Strings		 m_files;
};


class Texture
{
	friend class Renderer; // Only the Renderer can create new Texture objects!

private:
	Texture(); // can't instantiate directly; must ask Renderer to do it for you
	Texture(Texture const& copy) = delete; // No copying allowed!  This represents GPU memory.
	~Texture();

public:
	
	IntVec2				GetDimensions() const { return m_dimensions; }
	std::string const&	GetImageFilePath() const { return m_name; }
	const ResourceView*		GetOrCreateResourceViewByFlag(RESOURCE_BINDFLAGS flag);
	ResourceView*		GetResourceViewByFlag(RESOURCE_BINDFLAGS flag) const;

protected:
	TextureConfig		m_config;
	Renderer*			m_owner;
	std::string			m_name;
	IntVec2				m_dimensions;
	union 
	{
		ID3D11Resource*			m_texture = nullptr;
		ID3D11Texture2D*		m_texture2;
		ID3D11Texture3D*		m_texture3;
	};
	std::vector<ResourceView*>  m_views;
	//ID3D11ShaderResourceView*	m_shaderResourceView = nullptr;
};


