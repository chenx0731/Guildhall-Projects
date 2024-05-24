#include "Engine/Renderer/Texture.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include <d3d11.h>


Texture::Texture()
{
}

Texture::~Texture()
{
	m_owner = nullptr;
	DX_SAFE_RELEASE(m_texture);
	for (int viewIndex = 0; viewIndex < (int)m_views.size(); viewIndex++) {
		if (m_views[viewIndex]) {
			delete m_views[viewIndex];
			m_views[viewIndex] = nullptr;
		}
	}
	//DX_SAFE_RELEASE(m_shaderResourceView);
}

const ResourceView* Texture::GetOrCreateResourceViewByFlag(RESOURCE_BINDFLAGS flag)
{
	for (int viewIndex = 0; viewIndex < (int)m_views.size(); viewIndex++) {
		ResourceView*& view = m_views[viewIndex];
		if (view) {
			if (flag == view->m_config.m_flag) {
				return view;
			}
		}
	}
	ResourceViewConfig config;
	config.m_flag = flag;
	config.m_format = m_config.m_format;
	if (m_config.m_type == TextureType::CUBEMAP) {
		config.m_viewDimension = ViewDimension::TEXTURECUBE;
	}
	ResourceView* view = m_owner->GetOrCreateView(config, m_texture);
	m_views.push_back(view);
	return view;
}

ResourceView* Texture::GetResourceViewByFlag(RESOURCE_BINDFLAGS flag) const
{
	for (int viewIndex = 0; viewIndex < (int)m_views.size(); viewIndex++) {
		ResourceView* view = m_views[viewIndex];
		if (view) {
			if (flag == view->m_config.m_flag) {
				return view;
			}
		}
	}
	return nullptr;
}

