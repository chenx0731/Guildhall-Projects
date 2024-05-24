#include "Engine/Renderer/Shader.hpp"
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


Shader::Shader(const ShaderConfig& config) : m_config(config)
{
}


Shader::~Shader()
{
	DX_SAFE_RELEASE(m_vertexShader);
	DX_SAFE_RELEASE(m_geometryShader);
	DX_SAFE_RELEASE(m_pixelShader);
	DX_SAFE_RELEASE(m_computeShader);
	DX_SAFE_RELEASE(m_inputLayout);
}

const std::string& Shader::GetName() const
{
	// TODO: insert return statement here
	return m_config.m_name;
}
