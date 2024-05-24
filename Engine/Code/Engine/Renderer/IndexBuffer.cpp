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
#include "Engine/Renderer/IndexBuffer.hpp"
#include "Engine/Core/Vertex_PCU.hpp"



IndexBuffer::IndexBuffer(size_t size) : m_size(size)
{
}

IndexBuffer::~IndexBuffer()
{
	DX_SAFE_RELEASE(m_buffer);

}

unsigned int IndexBuffer::GetStride() const
{
	return sizeof(Vertex_PCU);
}
