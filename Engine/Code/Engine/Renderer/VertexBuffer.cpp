#include "Engine/Renderer/Renderer.hpp"
#define WIN32_LEAN_AND_MEAN
#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "dxgi.lib")
#pragma comment (lib, "d3dcompiler.lib")
#if defined(ENGINE_DEBUG_RENDER)
#include <dxgidebug.h>
#pragma comment(lib, "dxguid.lib")
#endif
#include <windows.h>
#include <d3d11.h>
#include <dxgi.h>
#include <d3dcompiler.h>
#include "Engine/Renderer/VertexBuffer.hpp"
#include "Engine/Core/Vertex_PCU.hpp"



VertexBuffer::VertexBuffer(size_t size, unsigned int stride) : m_size(size), m_stride(stride)
{
}

VertexBuffer::~VertexBuffer()
{
	DX_SAFE_RELEASE(m_buffer);

}

unsigned int VertexBuffer::GetStride() const
{
	return m_stride;
}


