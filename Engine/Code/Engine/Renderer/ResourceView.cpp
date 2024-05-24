#include "Engine/Renderer/ResourceView.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Renderer/Renderer.hpp"

#include <d3d11.h>


ResourceView::ResourceView(ResourceViewConfig config, ID3D11Resource* resource)
{
	m_config = config;
	m_resource = resource;
}

ResourceView::~ResourceView()
{
	DX_SAFE_RELEASE(m_view);
}

DXGI_FORMAT ResourceView::GetDX11FormatFromEnum(ResourceFormat format)
{
	switch (format)
	{
	case ResourceFormat::R24G8_TYPELESS:
		return DXGI_FORMAT_R24G8_TYPELESS;
	case ResourceFormat::D24_UNORM_S8_UINT:
		return DXGI_FORMAT_D24_UNORM_S8_UINT;
	case ResourceFormat::R8G8B8A8_UNORM:
		return DXGI_FORMAT_R8G8B8A8_UNORM;
	case ResourceFormat::R32G32B32A32_TYPELESS:
		return DXGI_FORMAT_R32G32B32A32_TYPELESS;
	case ResourceFormat::R32G32B32A32_FLOAT:
		return DXGI_FORMAT_R32G32B32A32_FLOAT;
	case ResourceFormat::R32_FLOAT:
		return DXGI_FORMAT_R32_FLOAT;
	case ResourceFormat::R32G32_FlOAT:
		return DXGI_FORMAT_R32G32_FLOAT;
	case ResourceFormat::UNKNOWN:
		return DXGI_FORMAT_UNKNOWN;
	default:
		ERROR_AND_DIE(Stringf("Error: Resource format:%d", (int)format));
		break;
	}
}

DXGI_FORMAT ResourceView::GetDX11FormatColorFromEnum(ResourceFormat format)
{
	switch (format)
	{
	case ResourceFormat::R24G8_TYPELESS:
		return DXGI_FORMAT_R24_UNORM_X8_TYPELESS;
	default:
		return GetDX11FormatFromEnum(format);
		break;
	}
}

unsigned int ResourceView::GetDX11BindflagsFromUINT(unsigned int bindflags)
{
	unsigned int result = 0;
	if (bindflags & RESOURCE_BINDFLAGS_DEPTH_STENCIL_VIEW) {
		result |= D3D11_BIND_DEPTH_STENCIL;
	}
	if (bindflags & RESOURCE_BINDFLAGS_RENDER_TARGET_VIEW) {
		result |= D3D11_BIND_RENDER_TARGET;
	}
	if (bindflags & RESOURCE_BINDFLAGS_SHADER_RESOURCE_VIEW) {
		result |= D3D11_BIND_SHADER_RESOURCE;
	}
	if (bindflags & RESOURCE_BINDFLAGS_UNORDERED_ACCESS_VIEW) {
		result |= D3D11_BIND_UNORDERED_ACCESS;
	}
	return result;
}

D3D11_USAGE ResourceView::GetDX11UsageFromMemoryUsage(MemoryUsage usage)
{
	switch (usage)
	{
	case MemoryUsage::DEFAULT:
		return D3D11_USAGE_DEFAULT;
	case MemoryUsage::DYNAMIC:
		return D3D11_USAGE_DYNAMIC;
	case MemoryUsage::IMMUTABLE:
		return D3D11_USAGE_IMMUTABLE;
	default:
		ERROR_AND_DIE(Stringf("Error: can't find usage:%d", (int)usage));
		break;
	}
}
