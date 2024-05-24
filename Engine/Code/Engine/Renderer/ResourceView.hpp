#pragma once

struct ID3D11Resource;
struct ID3D11View;
struct ID3D11UnorderedAccessView;
struct ID3D11ShaderResourceView;
struct ID3D11RenderTargetView;	
struct ID3D11DepthStencilView;
enum DXGI_FORMAT : int;
enum D3D11_USAGE : int;

enum RESOURCE_BINDFLAGS : unsigned int {
	RESOURCE_BINDFLAGS_SHADER_RESOURCE_VIEW = 1,
	RESOURCE_BINDFLAGS_UNORDERED_ACCESS_VIEW = 2,
	RESOURCE_BINDFLAGS_RENDER_TARGET_VIEW = 4,
	RESOURCE_BINDFLAGS_DEPTH_STENCIL_VIEW = 8,
};

enum class ResourceFormat
{
	NONE,
	R8G8B8A8_UNORM,
	R24G8_TYPELESS,
	D24_UNORM_S8_UINT,
	R32G32B32A32_TYPELESS,
	R32G32B32A32_FLOAT,
	R32_FLOAT,
	R32G32_FlOAT,
	UNKNOWN,
};

enum class MemoryUsage
{
	IMMUTABLE,
	DEFAULT,
	DYNAMIC,
};

enum class ViewDimension
{
	TEXTURE2D,
	TEXTURECUBE
};

struct ResourceViewConfig {
	RESOURCE_BINDFLAGS m_flag;
	ResourceFormat m_format;
	int			   m_numOfElements;
	ViewDimension  m_viewDimension = ViewDimension::TEXTURE2D;
};

class ResourceView
{
	friend class Renderer;
public:
	ResourceView(ResourceViewConfig config, ID3D11Resource* resource);
	~ResourceView();
	static DXGI_FORMAT GetDX11FormatFromEnum(ResourceFormat format);
	static DXGI_FORMAT GetDX11FormatColorFromEnum(ResourceFormat format);
	static unsigned int GetDX11BindflagsFromUINT(unsigned int bindflags);
	static D3D11_USAGE GetDX11UsageFromMemoryUsage(MemoryUsage usage);

public:
	ResourceViewConfig m_config;
	ID3D11Resource* m_resource = nullptr;
	//RESOURCE_BINDFLAGS m_flag;
	union
	{
		ID3D11View* m_view = nullptr;
		ID3D11UnorderedAccessView* m_UAV;
		ID3D11ShaderResourceView*  m_SRV;
		ID3D11RenderTargetView*	   m_RTV;
		ID3D11DepthStencilView*	   m_DSV;
	};
	inline bool operator==(ResourceView const& other) const {
		return m_config.m_flag == other.m_config.m_flag;
	}

};



