#include "Engine/Renderer/StructuredBuffer.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include <d3d11.h>


StructuredBuffer::StructuredBuffer()
{
}

StructuredBuffer::~StructuredBuffer()
{
	m_owner = nullptr;
	DX_SAFE_RELEASE(m_buffer);
	for (int viewIndex = 0; viewIndex < (int)m_views.size(); viewIndex++) {
		if (m_views[viewIndex]) {
			delete m_views[viewIndex];
			m_views[viewIndex] = nullptr;
		}
	}
	//DX_SAFE_RELEASE(m_shaderResourceView);
}

ResourceView* StructuredBuffer::GetOrCreateResourceViewByFlag(RESOURCE_BINDFLAGS flag)
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
	config.m_numOfElements = (int)m_config.m_numElements;
	ResourceView* view = m_owner->GetOrCreateView(config, m_buffer);
	m_views.push_back(view);
	return view;
}


