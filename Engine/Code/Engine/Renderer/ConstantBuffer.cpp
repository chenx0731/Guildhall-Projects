#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/ConstantBuffer.hpp"
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


ConstantBuffer::ConstantBuffer(size_t size) : m_size(size)
{
}

ConstantBuffer::~ConstantBuffer()
{
	DX_SAFE_RELEASE(m_buffer);
}
