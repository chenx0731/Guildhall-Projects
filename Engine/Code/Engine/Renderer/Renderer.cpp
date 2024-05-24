#include "ThirdParty/stb/stb_image.h"
#define UNUSED(x) (void)(x);
#define WIN32_LEAN_AND_MEAN
#include "Engine/Renderer/Renderer.hpp"
#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "dxgi.lib")
#pragma comment (lib, "d3dcompiler.lib")
#pragma comment(lib, "dxguid.lib")
#if defined(ENGINE_DEBUG_RENDER)
#include <dxgidebug.h>
#endif
#include <windows.h>
#include <d3d11.h>
#include <dxgi.h>
#include <d3dcompiler.h>
#if defined(OPAQUE)
#undef OPAQUE
#endif
#include "Engine/Renderer/Window.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/Mat44.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/Shader.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/FileUtils.hpp"
#include "Engine/Core/Image.hpp"
#include "Engine/Core/Vertex_PNCU.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Renderer/VertexBuffer.hpp"
#include "Engine/Renderer/IndexBuffer.hpp"
#include "Engine/Renderer/ConstantBuffer.hpp"
#include "Engine/Renderer/DefaultShader.hpp"
#include "Engine/Renderer/UnorderedAccessView.hpp"
#include "Engine/Renderer/GPUMesh.hpp"
#include "Engine/Renderer/CPUMesh.hpp"
#include "Engine/Renderer/ResourceView.hpp"
#include "Engine/Renderer/StructuredBuffer.hpp"

struct LightConstants
{
	Vec3 SunDirection;
	float SunIntensity;
	float AmbientIntensity[4];
	Vec3 WorldEyePosition;
	int NormalMode;
	int SpecularMode;
	float SpecularIntensity;
	float SpecularPower;
	float Padding;
};
static const int k_lightConstantsSlot = 1;

struct CameraConstants
{
	Mat44 ProjectionMatrix;
	Mat44 ViewMatrix;
	Mat44 InvProjectionMatrix;
	Mat44 InvViewMatrix;
};
static const int k_cameraConstantsSlot = 2;

struct ModelConstants
{
	Mat44 ModelMatrix;
	float ModelColor[4];
};
static const int k_modelConstantsSlot = 3;

D3D_PRIMITIVE_TOPOLOGY GetPrimitiveTopology(Topology topology)
{
	switch (topology)
	{
	case Topology::LineList: {
		return D3D11_PRIMITIVE_TOPOLOGY_LINELIST;
	}

	case Topology::TriangleList: {
		return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	}
	case Topology::PointList: {
		return D3D11_PRIMITIVE_TOPOLOGY_POINTLIST;
	}
	default: {
		return D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST;
	}
	}
}

Renderer::Renderer(RendererConfig const& config) : m_config(config)
{
}

Renderer::~Renderer()
{
}

void Renderer::Startup()
{
	//CreateRenderContext();

	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
#if defined(ENGINE_DEBUG_RENDER)

	m_dxgiDebugModule = (void*) ::LoadLibraryA("dxgidebug.dll");
	if (m_dxgiDebugModule == nullptr)
	{
		ERROR_AND_DIE("Could not load dxgidebug.dll");
	}

	typedef HRESULT(WINAPI* GetDebugModuleCB)(REFIID, void**);
	((GetDebugModuleCB)::GetProcAddress((HMODULE)m_dxgiDebugModule, "DXGIGetDebugInterface"))(__uuidof(IDXGIDebug), &m_dxgiDebug);

	if (m_dxgiDebug == nullptr)
	{
		ERROR_AND_DIE("Could not load debug module");
	}
#endif

	DXGI_SWAP_CHAIN_DESC	swapChainDesc = { 0 };
	swapChainDesc.BufferDesc.Width = m_config.m_window->GetClientDimensions().x;
	swapChainDesc.BufferDesc.Height = m_config.m_window->GetClientDimensions().y;
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = 2;
	swapChainDesc.OutputWindow = (HWND)(m_config.m_window->GetHwnd());
	swapChainDesc.Windowed = true;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;

	HRESULT hr;
	D3D_FEATURE_LEVEL level = {};
	level = D3D_FEATURE_LEVEL_11_1;
	hr = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL,
		D3D11_CREATE_DEVICE_DEBUG, &level, 1, D3D11_SDK_VERSION,
		&swapChainDesc, &m_swapChain, &m_device, NULL, &m_deviceContext);

	if (!SUCCEEDED(hr)) {
		ERROR_AND_DIE("Could not create device and swap chain.");
	}


	// Get back buffer texture
	ID3D11Texture2D* backBuffer;
	//HRESULT hr;
	hr = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&backBuffer);
	if (!SUCCEEDED(hr)) {
		ERROR_AND_DIE("Could not get swap chain buffer.");
	}

	hr = m_device->CreateRenderTargetView(backBuffer, NULL, &m_targetView);

	TextureConfig preBackBuffer;
	preBackBuffer.m_name = "Pre Back Buffer";
	preBackBuffer.m_width = m_config.m_window->GetClientDimensions().x;
		//m_worldCamera.m_viewport.GetDimensions().x;
	preBackBuffer.m_height = m_config.m_window->GetClientDimensions().y;
	preBackBuffer.m_format = ResourceFormat::R8G8B8A8_UNORM;
	preBackBuffer.m_bindFlags = RESOURCE_BINDFLAGS_SHADER_RESOURCE_VIEW | RESOURCE_BINDFLAGS_RENDER_TARGET_VIEW;
	preBackBuffer.m_usage = MemoryUsage::DEFAULT;

	m_preBackBuffer = g_theRenderer->CreateTextureByConfig(preBackBuffer);
	m_preBackBuffer->GetOrCreateResourceViewByFlag(RESOURCE_BINDFLAGS_RENDER_TARGET_VIEW);
	m_preBackBuffer->GetOrCreateResourceViewByFlag(RESOURCE_BINDFLAGS_SHADER_RESOURCE_VIEW);

	if (!SUCCEEDED(hr)) {
		ERROR_AND_DIE("Could create render target view for swap chain buffer.");
	}

	//backBuffer->Release();
	DX_SAFE_RELEASE(backBuffer);

	m_defaultShader = CreateShader("Data/Shaders/Default");
	m_copyShader	= CreateShader("Data/Shaders/CopyToDefaultTargetView");
		//CreateShader("Default", g_defaultShader);
	Image defaultImage = Image(IntVec2(2, 2), Rgba8::WHITE);
	m_defaultTexture = CreateTextureFromImage(defaultImage);
	//m_currentShader = CreateShader("Data/Shaders/Default");
	//m_immediateCBO = CreateConstantBuffer(16);
	m_immediateVBO_PCU = CreateVertexBuffer(sizeof(Vertex_PCU), sizeof(Vertex_PCU));
	m_immediateVBO_PNCU = CreateVertexBuffer(sizeof(Vertex_PCUTBN), sizeof(Vertex_PCUTBN));
	m_immediateIBO = CreateIndexBuffer(sizeof(unsigned int));
	m_lightCBO = CreateConstantBuffer(sizeof(LightConstants));
	m_cameraCBO = CreateConstantBuffer(sizeof(CameraConstants));
	m_modelCBO = CreateConstantBuffer(sizeof(ModelConstants));

	// create rasterizer modes
	CreateRasterizerStates();

	BindShader(m_currentShader);
	BindTexture(m_defaultTexture);

	// create blend states
	CreateBlendStates();
	

	// sampler state
	CreateSamplerStates();

	TextureConfig config;
	config.m_name = "Depth Stencil Texture";
	config.m_width = m_config.m_window->GetClientDimensions().x;
	config.m_height = m_config.m_window->GetClientDimensions().y;
	config.m_format = ResourceFormat::R24G8_TYPELESS;
	config.m_bindFlags = RESOURCE_BINDFLAGS_SHADER_RESOURCE_VIEW | RESOURCE_BINDFLAGS_DEPTH_STENCIL_VIEW;
	config.m_usage = MemoryUsage::DEFAULT;

	m_depthStencilTexture = CreateTextureByConfig(config);

	CreateDepthStates();

	SetStatesIfChanged();
}

void Renderer::BeginFrame()
{
	m_deviceContext->OMSetRenderTargets(1, &m_preBackBuffer->GetResourceViewByFlag(RESOURCE_BINDFLAGS_RENDER_TARGET_VIEW)->m_RTV, m_depthStencilTexture->GetOrCreateResourceViewByFlag(RESOURCE_BINDFLAGS_DEPTH_STENCIL_VIEW)->m_DSV);

}

void Renderer::EndFrame()
{
	HRESULT hr;

	m_deviceContext->OMSetRenderTargets(1, &m_targetView, m_depthStencilTexture->GetOrCreateResourceViewByFlag(RESOURCE_BINDFLAGS_DEPTH_STENCIL_VIEW)->m_DSV);

	BindShader(m_copyShader);
	BindTexture(m_preBackBuffer, 2);
	Draw(3);
	BindShader(nullptr);
	BindTexture(nullptr, 2);

	hr = m_swapChain->Present(0, 0);
	if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
	{
		ERROR_AND_DIE("Device has been lost, application will be terminated.");
	}
	Rgba8 m_color = Rgba8(0, 0, 0, 0);
	float m_colorRGBA[4];
	m_color.GetAsFloats(m_colorRGBA);
	m_deviceContext->ClearRenderTargetView(m_targetView, m_colorRGBA);
}
void Renderer::Shutdown()
{
	for (int shaderIndex = 0; shaderIndex < m_loadedShaders.size(); shaderIndex++) {
	 	delete m_loadedShaders[shaderIndex];
		m_loadedShaders[shaderIndex] = nullptr;
	}
	
	m_currentShader = nullptr;
	m_defaultShader = nullptr;
	m_copyShader = nullptr;

	for (int textureIndex = 0; textureIndex < m_loadedTextures.size(); textureIndex++) {
		delete m_loadedTextures[textureIndex];
		m_loadedTextures[textureIndex] = nullptr;
	}

	m_depthStencilTexture = nullptr;

	m_defaultTexture = nullptr;

	m_preBackBuffer = nullptr;

	for (int bitmapIndex = 0; bitmapIndex < m_loadedFonts.size(); bitmapIndex++) {
		delete m_loadedFonts[bitmapIndex];
		m_loadedFonts[bitmapIndex] = nullptr;
	}

	delete m_immediateVBO_PCU;
	m_immediateVBO_PCU = nullptr;

	delete m_immediateVBO_PNCU;
	m_immediateVBO_PNCU = nullptr;

	delete m_immediateIBO;
	m_immediateIBO = nullptr;

	delete m_lightCBO;
	m_lightCBO = nullptr;

	delete m_cameraCBO;
	m_cameraCBO = nullptr;

	delete m_modelCBO;
	m_modelCBO = nullptr;

	for (int i = 0; i < (int)BlendMode::COUNT; i++) {
		DX_SAFE_RELEASE(m_blendStates[i]);
	}
	for (int i = 0; i < (int)RasterizerMode::COUNT; i++) {
		DX_SAFE_RELEASE(m_rasterizerStates[i]);
	}
	for (int i = 0; i < (int)SamplerMode::COUNT; i++) {
		DX_SAFE_RELEASE(m_samplerStates[i]);
	}
	for (int i = 0; i < (int)DepthMode::COUNT; i++) {
		DX_SAFE_RELEASE(m_depthStencilStates[i]);
	}

	m_rasterizerState = nullptr;
	m_blendState = nullptr;
	m_samplerState = nullptr;
	m_depthStencilState = nullptr;

	DX_SAFE_RELEASE(m_device);
	DX_SAFE_RELEASE(m_deviceContext);
	DX_SAFE_RELEASE(m_swapChain);
	DX_SAFE_RELEASE(m_targetView);
	//DX_SAFE_RELEASE(m_depthStencilView);
	//DX_SAFE_RELEASE(m_depthStencilTexture);

#if defined(ENGINE_DEBUG_RENDER)
	((IDXGIDebug*)m_dxgiDebug)->ReportLiveObjects(DXGI_DEBUG_ALL, (DXGI_DEBUG_RLO_FLAGS)(DXGI_DEBUG_RLO_DETAIL | DXGI_DEBUG_RLO_IGNORE_INTERNAL));
	((IDXGIDebug*)m_dxgiDebug)->Release();
	m_dxgiDebug = nullptr;
	::FreeLibrary((HMODULE)m_dxgiDebugModule);
	m_dxgiDebugModule = nullptr;
#endif
}

//void Renderer::ClearScreen(const Rgba8& clearColor)
//{
//	Rgba8 m_color = clearColor;
//	float m_colorRGBA[4];
//	m_color.GetAsFloats(m_colorRGBA);
//	m_deviceContext->ClearRenderTargetView(m_targetView, m_colorRGBA);
//	m_deviceContext->ClearDepthStencilView(m_depthStencilTexture->GetOrCreateResourceViewByFlag(RESOURCE_BINDFLAGS_DEPTH_STENCIL_VIEW)->m_DSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1, 0);
//}

void Renderer::ClearScreen(const Rgba8& clearColor, Texture const* depthStencil, Texture const* renderTarget)
{
	Rgba8 m_color = clearColor;
	float m_colorRGBA[4];
	m_color.GetAsFloats(m_colorRGBA);
	if (renderTarget == nullptr)
		m_deviceContext->ClearRenderTargetView(m_preBackBuffer->GetResourceViewByFlag(RESOURCE_BINDFLAGS_RENDER_TARGET_VIEW)->m_RTV, m_colorRGBA);
	else m_deviceContext->ClearRenderTargetView(renderTarget->GetResourceViewByFlag(RESOURCE_BINDFLAGS_RENDER_TARGET_VIEW)->m_RTV, m_colorRGBA);
	if (depthStencil)
	m_deviceContext->ClearDepthStencilView(depthStencil->GetResourceViewByFlag(RESOURCE_BINDFLAGS_DEPTH_STENCIL_VIEW)->m_DSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1, 0);
	else {
		m_deviceContext->ClearDepthStencilView(m_depthStencilTexture->GetOrCreateResourceViewByFlag(RESOURCE_BINDFLAGS_DEPTH_STENCIL_VIEW)->m_DSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1, 0);
	}
}

void Renderer::ClearRenderTarget(const Rgba8& clearColor, Texture const* renderTarget /*= nullptr*/)
{
	Rgba8 l_color = clearColor;
	float l_colorRGBA[4];
	l_color.GetAsFloats(l_colorRGBA);
	if (renderTarget == nullptr)
		m_deviceContext->ClearRenderTargetView(m_preBackBuffer->GetResourceViewByFlag(RESOURCE_BINDFLAGS_RENDER_TARGET_VIEW)->m_RTV, l_colorRGBA);
	else m_deviceContext->ClearRenderTargetView(renderTarget->GetResourceViewByFlag(RESOURCE_BINDFLAGS_RENDER_TARGET_VIEW)->m_RTV, l_colorRGBA);
}

void Renderer::ClearDepthStencil(const Rgba8& clearColor, Texture const* depthStencil /*= nullptr*/)
{
	Rgba8 m_color = clearColor;
	float m_colorRGBA[4];
	m_color.GetAsFloats(m_colorRGBA);
	if (depthStencil)
		m_deviceContext->ClearDepthStencilView(depthStencil->GetResourceViewByFlag(RESOURCE_BINDFLAGS_DEPTH_STENCIL_VIEW)->m_DSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1, 0);
	else {
		m_deviceContext->ClearDepthStencilView(m_depthStencilTexture->GetOrCreateResourceViewByFlag(RESOURCE_BINDFLAGS_DEPTH_STENCIL_VIEW)->m_DSV, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1, 0);
	}
}

void Renderer::BeginCamera(const Camera& camera)
{
	D3D11_VIEWPORT viewPort = { 0 };
	viewPort.TopLeftX = camera.m_viewport.m_mins.x;
	viewPort.TopLeftY = camera.m_viewport.m_mins.y;
	viewPort.Width = camera.m_viewport.GetDimensions().x;
		//(float)m_config.m_window->GetClientDimensions().x;
	viewPort.Height = camera.m_viewport.GetDimensions().y;
		//(float)m_config.m_window->GetClientDimensions().y;
	viewPort.MinDepth = 0;
	viewPort.MaxDepth = 1;

	m_deviceContext->RSSetViewports(1, &viewPort);

	CameraConstants l_cameraConstants = {};
	l_cameraConstants.ProjectionMatrix = camera.GetProjectionMatrix();
	l_cameraConstants.ViewMatrix = camera.GetViewMatrix();
	l_cameraConstants.InvProjectionMatrix = l_cameraConstants.ProjectionMatrix.GetInversedMatrix();
	l_cameraConstants.InvViewMatrix = l_cameraConstants.ViewMatrix.GetInversedMatrix();

	CopyCPUToGPU(&l_cameraConstants, sizeof(CameraConstants), m_cameraCBO);
	BindConstantBuffer(k_cameraConstantsSlot, m_cameraCBO);
	CSBindConstantBuffer(k_cameraConstantsSlot, m_cameraCBO);

	// TODO: Remove it to somewhere it belongs to
	//SetLightConstants();
	//SetStatesIfChanged();
}

void Renderer::EndCamera(const Camera& camera)
{
	UNUSED(camera);
}

void Renderer::DispatchCS(int threadX, int threadY, int threadZ)
{
	m_deviceContext->Dispatch((UINT)threadX, (UINT)threadY, (UINT)threadZ);
}

void Renderer::DrawVertexArray(int numVertexes, const Vertex_PCU* vertexes, Topology topology)
{
	CopyCPUToGPU(vertexes, numVertexes * sizeof(Vertex_PCU), sizeof(Vertex_PCU), m_immediateVBO_PCU);
	//m_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	DrawVertexBuffer(m_immediateVBO_PCU, numVertexes, topology);
}

void Renderer::DrawVertexArray(int numVertexes, const Vertex_PCUTBN* vertexes)
{
	CopyCPUToGPU(vertexes, numVertexes * sizeof(Vertex_PCUTBN), sizeof(Vertex_PCUTBN), m_immediateVBO_PNCU);
	//m_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	DrawVertexBuffer(m_immediateVBO_PNCU, numVertexes);
}

void Renderer::DrawVertexIndexArray(int numVertexes, int numIndexes, const Vertex_PCU* vertexes, const unsigned int* indexes, VertexBuffer* vertexBuffer, IndexBuffer* indexBuffer)
{
	if (vertexBuffer)
		CopyCPUToGPU(vertexes, numVertexes * sizeof(Vertex_PCU), sizeof(Vertex_PCU), vertexBuffer);
	else CopyCPUToGPU(vertexes, numVertexes * sizeof(Vertex_PCU), sizeof(Vertex_PCU), m_immediateVBO_PCU);

	if (indexBuffer)
		CopyCPUToGPU(indexes, numIndexes * sizeof(unsigned int), indexBuffer);
	else CopyCPUToGPU(indexes, numIndexes * sizeof(unsigned int), m_immediateIBO);

	if (vertexBuffer)
		BindVertexBuffer(vertexBuffer);
	else BindVertexBuffer(m_immediateVBO_PCU);

	if (indexBuffer)
		BindIndexBuffer(indexBuffer);
	else BindIndexBuffer(m_immediateIBO);

	m_deviceContext->DrawIndexed(numIndexes, 0, 0);
}

void Renderer::DrawVertexIndexArray(int numVertexes, int numIndexes, const Vertex_PCUTBN* vertexes, const unsigned int* indexes, VertexBuffer* vertexBuffer, IndexBuffer* indexBuffer)
{
	//m_deviceContext->IASetPrimitiveTopology(vertexBuffer->GetPrimitiveTopology());
	if (vertexBuffer)
		CopyCPUToGPU(vertexes, numVertexes * sizeof(Vertex_PCUTBN), sizeof(Vertex_PCUTBN), vertexBuffer);
	else CopyCPUToGPU(vertexes, numVertexes * sizeof(Vertex_PCU), sizeof(Vertex_PCU), m_immediateVBO_PNCU);

	if (indexBuffer)
		CopyCPUToGPU(indexes, numIndexes * sizeof(unsigned int), indexBuffer);
	else CopyCPUToGPU(indexes, numIndexes * sizeof(unsigned int), m_immediateIBO);

	if (vertexBuffer)
		BindVertexBuffer(vertexBuffer);
	else BindVertexBuffer(m_immediateVBO_PNCU);

	if (indexBuffer)
		BindIndexBuffer(indexBuffer);
	else BindIndexBuffer(m_immediateIBO);

	m_deviceContext->DrawIndexed(numIndexes, 0, 0);
}

void Renderer::DrawVertexBuffer(VertexBuffer* vbo, int vertexCount, Topology topology, int vertexOffset)
{
	//m_deviceContext->IASetPrimitiveTopology(GetPrimitiveTopology(topology));
	BindVertexBuffer(vbo, topology);
	m_deviceContext->Draw(vertexCount, vertexOffset);

}

void Renderer::DrawVertexIndexBuffer(VertexBuffer* vbo, IndexBuffer* ibo, size_t indexCount)
{
	BindVertexBuffer(vbo);
	BindIndexBuffer(ibo);
	m_deviceContext->DrawIndexed((UINT)indexCount, 0, 0);
}

void Renderer::Draw(int vertexCount, int vertexOffset)
{
	
	m_deviceContext->Draw(vertexCount, vertexOffset);
}




void Renderer::DrawIndexed(unsigned int IndexCount, unsigned int StartIndexLocation, int BaseVertexLocation)
{
	m_deviceContext->DrawIndexed(IndexCount, StartIndexLocation, BaseVertexLocation);
}

Texture* Renderer::CreateOrGetTextureFromFile(char const* imageFilePath)
{
	// See if we already have this texture previously loaded
	Texture* existingTexture = GetTextureForFileName(imageFilePath);
	if (existingTexture)
	{
		return existingTexture;
	}

	// Never seen this texture before!  Let's load it.
	//Texture* newTexture = CreateTextureFromFile(imageFilePath);

	Image loadImage = Image(imageFilePath);
	Texture* newTexture = CreateTextureFromImage(loadImage);
	return newTexture;
}

Texture* Renderer::CreateTextureFromFile(char const* imageFilePath)
{
	IntVec2 dimensions = IntVec2(0, 0);		// This will be filled in for us to indicate image width & height
	int bytesPerTexel = 0; // This will be filled in for us to indicate how many color components the image had (e.g. 3=RGB=24bit, 4=RGBA=32bit)
	int numComponentsRequested = 0; // don't care; we support 3 (24-bit RGB) or 4 (32-bit RGBA)

	// Load (and decompress) the image RGB(A) bytes from a file on disk into a memory buffer (array of bytes)
	stbi_set_flip_vertically_on_load(1); // We prefer uvTexCoords has origin (0,0) at BOTTOM LEFT
	unsigned char* texelData = stbi_load(imageFilePath, &dimensions.x, &dimensions.y, &bytesPerTexel, numComponentsRequested);

	// Check if the load was successful
	GUARANTEE_OR_DIE(texelData, Stringf("Failed to load image \"%s\"", imageFilePath));

	Texture* newTexture = CreateTextureFromData(imageFilePath, dimensions, bytesPerTexel, texelData);

	// Free the raw image texel data now that we've sent a copy of it down to the GPU to be stored in video memory
	stbi_image_free(texelData);

	//m_loadedTextures.push_back(newTexture);
	return newTexture;
}

void Renderer::SetStatesIfChanged()
{
	//Set Blend State
	if (m_blendState != m_blendStates[(int)m_desiredBlendMode]) {
		m_blendState = m_blendStates[(int)m_desiredBlendMode];
		float blendFactor[4] = { 0.f };
		m_deviceContext->OMSetBlendState(m_blendState, blendFactor, 0xffffffff);
	}
	
	// Set Rasterizer State
	if (m_rasterizerState != m_rasterizerStates[(int)m_desiredRasterizerMode]) {
		m_rasterizerState = m_rasterizerStates[(int)m_desiredRasterizerMode];
		m_deviceContext->RSSetState(m_rasterizerState);
	}
	
	// Set Sampler State
	if (m_samplerState != m_samplerStates[(int)m_desiredSamplerMode]) {
		m_samplerState	= m_samplerStates[(int)m_desiredSamplerMode];
		m_deviceContext->PSSetSamplers(0, 1, &m_samplerState);
	}
	
	// Set Depth State
	if (m_depthStencilState != m_depthStencilStates[(int)m_desiredDepthMode]) {
		m_depthStencilState = m_depthStencilStates[(int)m_desiredDepthMode];
		m_deviceContext->OMSetDepthStencilState(m_depthStencilState, 0);
	}
	
}

void Renderer::SetBlendMode(BlendMode blendMode)
{
	m_desiredBlendMode = blendMode;
	SetStatesIfChanged();
}

void Renderer::SetSamplerMode(SamplerMode samplerMode)
{
	m_desiredSamplerMode = samplerMode;
	SetStatesIfChanged();
}

void Renderer::SetRasterizerMode(RasterizerMode rasterizerMode)
{
	m_desiredRasterizerMode = rasterizerMode;
	SetStatesIfChanged();
}

void Renderer::SetDepthMode(DepthMode depthMode)
{
	m_desiredDepthMode = depthMode;
	SetStatesIfChanged();
}

void Renderer::CSSetUAV(StructuredBuffer* buffer, int slot) const
{
	if (buffer) {
		m_deviceContext->CSSetUnorderedAccessViews(slot, 1, &buffer->GetOrCreateResourceViewByFlag(RESOURCE_BINDFLAGS_UNORDERED_ACCESS_VIEW)->m_UAV, NULL);
		
		//m_deviceContext->PSSetUnorderedAccessViews(slot, 1, &uav->m_UAV, NULL);
		//m_deviceContext->CSSetShaderResources(slot, 1, &uav->m_SRV);
	}
	else {
		ID3D11UnorderedAccessView* ppUAViewNULL[1] = { NULL };
		//ID3D11ShaderResourceView* ppSRViewNULL[1] = { NULL };
		m_deviceContext->CSSetUnorderedAccessViews(slot, 1, ppUAViewNULL, NULL);
		//m_deviceContext->VSSetUnorderedAccessViews(slot, 1, ppUAViewNULL, NULL);
		//m_deviceContext->CSSetShaderResources(slot, 1, ppSRViewNULL);
	}
}

void Renderer::CSSetUAV(Texture* texture, int slot) const
{
	if (texture) {
		m_deviceContext->CSSetUnorderedAccessViews(slot, 1, &texture->GetOrCreateResourceViewByFlag(RESOURCE_BINDFLAGS_UNORDERED_ACCESS_VIEW)->m_UAV, NULL);

		//m_deviceContext->PSSetUnorderedAccessViews(slot, 1, &uav->m_UAV, NULL);
		//m_deviceContext->CSSetShaderResources(slot, 1, &uav->m_SRV);
	}
	else {
		ID3D11UnorderedAccessView* ppUAViewNULL[1] = { NULL };
		//ID3D11ShaderResourceView* ppSRViewNULL[1] = { NULL };
		m_deviceContext->CSSetUnorderedAccessViews(slot, 1, ppUAViewNULL, NULL);
		//m_deviceContext->VSSetUnorderedAccessViews(slot, 1, ppUAViewNULL, NULL);
		//m_deviceContext->CSSetShaderResources(slot, 1, ppSRViewNULL);
	}
}

void Renderer::CSSetSRV(StructuredBuffer* buffer, int slot) const
{
	if (buffer) {
		m_deviceContext->CSSetShaderResources(slot, 1, &buffer->GetOrCreateResourceViewByFlag(RESOURCE_BINDFLAGS_SHADER_RESOURCE_VIEW)->m_SRV);
	}
	else {
		ID3D11ShaderResourceView* ppSRViewNULL[1] = { NULL };
		m_deviceContext->CSSetShaderResources(slot, 1, ppSRViewNULL);
	}
}

void Renderer::CSSetSRV(Texture* texture, int slot) const
{
	if (texture) {
		m_deviceContext->CSSetShaderResources(slot, 1, &texture->GetOrCreateResourceViewByFlag(RESOURCE_BINDFLAGS_SHADER_RESOURCE_VIEW)->m_SRV);
	}
	else {
		ID3D11ShaderResourceView* ppSRViewNULL[1] = { NULL };
		m_deviceContext->CSSetShaderResources(slot, 1, ppSRViewNULL);
	}
}

void Renderer::VSSetSRV(StructuredBuffer* buffer, int slot) const
{
	if (buffer) {
		m_deviceContext->VSSetShaderResources(slot, 1, &buffer->GetOrCreateResourceViewByFlag(RESOURCE_BINDFLAGS_SHADER_RESOURCE_VIEW)->m_SRV);
	}
	else {
		ID3D11ShaderResourceView* ppSRViewNULL[1] = { NULL };
		m_deviceContext->VSSetShaderResources(slot, 1, ppSRViewNULL);
	}
}


void Renderer::VSSetSRV(Texture* buffer, int slot) const
{
	if (buffer) {
		m_deviceContext->VSSetShaderResources(slot, 1, &buffer->GetOrCreateResourceViewByFlag(RESOURCE_BINDFLAGS_SHADER_RESOURCE_VIEW)->m_SRV);
	}
	else {
		ID3D11ShaderResourceView* ppSRViewNULL[1] = { NULL };
		m_deviceContext->VSSetShaderResources(slot, 1, ppSRViewNULL);
	}
}

//void Renderer::PSSetUAV(Texture* texture, int slot) const
//{
//	if (texture) {
//		m_deviceContext->VSSetShaderResources(slot, 1, &texture->GetOrCreateResourceViewByFlag(RESOURCE_BINDFLAGS_UNORDERED_ACCESS_VIEW)->m_UAV);
//	}
//	else {
//		ID3D11ShaderResourceView* ppSRViewNULL[1] = { NULL };
//		m_deviceContext->VSSetShaderResources(slot, 1, ppSRViewNULL);
//	}
//}

void Renderer::SetRenderTarget() const
{
	ID3D11RenderTargetView* temp = nullptr;
	m_deviceContext->OMSetRenderTargets(1, &temp, NULL);
}

void Renderer::SetTextureAsRenderTarget(const Texture* texture) const
{
	if (texture)
		m_deviceContext->OMSetRenderTargets(1, &texture->GetResourceViewByFlag(RESOURCE_BINDFLAGS_RENDER_TARGET_VIEW)->m_RTV, m_depthStencilTexture->GetOrCreateResourceViewByFlag(RESOURCE_BINDFLAGS_DEPTH_STENCIL_VIEW)->m_DSV);
	else m_deviceContext->OMSetRenderTargets(1, &m_preBackBuffer->GetResourceViewByFlag(RESOURCE_BINDFLAGS_RENDER_TARGET_VIEW)->m_RTV, m_depthStencilTexture->GetOrCreateResourceViewByFlag(RESOURCE_BINDFLAGS_DEPTH_STENCIL_VIEW)->m_DSV);
}

void Renderer::UpdateConstantBuffer(ConstantBuffer* cbo, const void* gpuData, size_t size)
{
	CopyCPUToGPU(gpuData, size, cbo);
}

void Renderer::SetModelConstants(const Mat44& modelMatrix, const Rgba8& modelColor)
{
	ModelConstants modelConstants = {};
	modelConstants.ModelMatrix = modelMatrix;
	modelColor.GetAsFloats(modelConstants.ModelColor);

	CopyCPUToGPU(&modelConstants, sizeof(ModelConstants), m_modelCBO);
	BindConstantBuffer(k_modelConstantsSlot, m_modelCBO);
}

void Renderer::SetLightConstants(const Vec3& sunDirection, const float sunIntensity, const Rgba8& ambientColor,
	Vec3 worldEyePos, int normalMode, int specularMode, float specularIntensity, float specularPower)
{
	LightConstants lightConstant = {};
	lightConstant.SunDirection = sunDirection;
	//lightConstant.SunDirection = Mat44();
	lightConstant.SunIntensity = sunIntensity;
	ambientColor.GetAsFloats(lightConstant.AmbientIntensity);

	lightConstant.WorldEyePosition = worldEyePos;
	lightConstant.NormalMode = normalMode;
	lightConstant.SpecularMode = specularMode;
	lightConstant.SpecularIntensity = specularIntensity;
	lightConstant.SpecularPower = specularPower;

	CopyCPUToGPU(&lightConstant, sizeof(LightConstants), m_lightCBO);
	BindConstantBuffer(k_lightConstantsSlot, m_lightCBO);
}

VertexBuffer* Renderer::CreateVertexBuffer(const size_t size, const unsigned int stride)
{
	VertexBuffer* newVertexBuffer = new VertexBuffer(size, stride);

	D3D11_BUFFER_DESC bufferDesc = { 0 };
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.ByteWidth = (UINT)size;
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	//ID3D11Buffer* vertexBuffer = nullptr;

	HRESULT	hr = m_device->CreateBuffer(&bufferDesc, NULL, &newVertexBuffer->m_buffer);
	if (!SUCCEEDED(hr)) {
		ERROR_AND_DIE("Could not create vertex buffer.");
	}
	std::string debugName = Stringf("Vertex Buffer - size: %d", size);
	SetObjDebugName(newVertexBuffer->m_buffer, debugName.c_str());
	return newVertexBuffer;
}

IndexBuffer* Renderer::CreateIndexBuffer(const size_t size)
{
	IndexBuffer* newIndexBuffer = new IndexBuffer(size);

	D3D11_BUFFER_DESC bufferDesc = { 0 };
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.ByteWidth = (UINT)size;
	bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	//ID3D11Buffer* vertexBuffer = nullptr;

	HRESULT	hr = m_device->CreateBuffer(&bufferDesc, NULL, &newIndexBuffer->m_buffer);
	if (!SUCCEEDED(hr)) {
		ERROR_AND_DIE("Could not create index buffer.");
	}
	std::string debugName = Stringf("Index Buffer - size: %d", size);
	SetObjDebugName(newIndexBuffer->m_buffer, debugName.c_str());
	return newIndexBuffer;
}

ID3D11Buffer* Renderer::CreateOutputBuffer(const size_t size, const unsigned int stride)
{
	D3D11_BUFFER_DESC bufferDesc = { 0 };
	bufferDesc.Usage = D3D11_USAGE_STAGING;
	bufferDesc.ByteWidth = (UINT)size * stride;
	bufferDesc.BindFlags = 0;
	bufferDesc.StructureByteStride = stride;

	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
	bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

	ID3D11Buffer* outputBuffer = nullptr;

	HRESULT	hr = m_device->CreateBuffer(&bufferDesc, NULL, &outputBuffer);
	if (!SUCCEEDED(hr)) {
		ERROR_AND_DIE("Could not create output buffer.");
	}
	std::string debugName = Stringf("Output Buffer - size: %d", size);
	SetObjDebugName(outputBuffer, debugName.c_str());
	return outputBuffer;
}

ID3D11Buffer* Renderer::CreateInputBuffer(const size_t size, const unsigned int stride)
{
	D3D11_BUFFER_DESC bufferDesc = { 0 };
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.ByteWidth = (UINT)size * stride;
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.StructureByteStride = stride;

	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	//bufferDesc.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

	ID3D11Buffer* inputBuffer = nullptr;

	HRESULT	hr = m_device->CreateBuffer(&bufferDesc, NULL, &inputBuffer);
	if (!SUCCEEDED(hr)) {
		ERROR_AND_DIE("Could not create input buffer.");
	}
	std::string debugName = Stringf("Input Buffer - size: %d", size);
	SetObjDebugName(inputBuffer, debugName.c_str());
	return inputBuffer;
}

GPUMesh* Renderer::CreateGPUMeshFromCPUMesh(const CPUMesh* cpuMesh)
{
	VertexBuffer* vbo = CreateVertexBuffer(sizeof(Vertex_PCUTBN) * cpuMesh->m_vertexes.size(), sizeof(Vertex_PCUTBN));
	IndexBuffer* ibo = CreateIndexBuffer(sizeof(unsigned int) * cpuMesh->m_indexes.size());
	CopyCPUToGPU(cpuMesh->m_vertexes.data(), sizeof(Vertex_PCUTBN) * cpuMesh->m_vertexes.size(), sizeof(Vertex_PCUTBN), vbo);
	CopyCPUToGPU(cpuMesh->m_indexes.data(), sizeof(unsigned int) * cpuMesh->m_indexes.size(), ibo);
	GPUMesh* gpuMesh = new GPUMesh(vbo, ibo);
	return gpuMesh;
}

void Renderer::CopyCPUToGPU(const void* data, size_t size, unsigned int stride, VertexBuffer*& vbo)
{
	if (vbo->m_size < size) {
		delete vbo;
		vbo = nullptr;
		vbo = CreateVertexBuffer(size, stride);
	}
	
	D3D11_MAPPED_SUBRESOURCE mappedSubresource;

	HRESULT hr = m_deviceContext->Map(vbo->m_buffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &mappedSubresource);
	if (!SUCCEEDED(hr)) {
		ERROR_AND_DIE("Could not map content.");
	}

	memcpy(mappedSubresource.pData, data, size);
	m_deviceContext->Unmap(vbo->m_buffer, NULL);
}

void Renderer::CopyCPUToGPU(const void* data, size_t size, IndexBuffer*& ibo)
{
	if (ibo->m_size < size) {
		delete ibo;
		ibo = nullptr;
		ibo = CreateIndexBuffer(size);
	}

	D3D11_MAPPED_SUBRESOURCE mappedSubresource;

	HRESULT hr = m_deviceContext->Map(ibo->m_buffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &mappedSubresource);
	if (!SUCCEEDED(hr)) {
		ERROR_AND_DIE("Could not map content.");
	}

	memcpy(mappedSubresource.pData, data, size);
	m_deviceContext->Unmap(ibo->m_buffer, NULL);
}

void Renderer::CopyResources(void* dst, const void* src)
{
	m_deviceContext->CopyResource((ID3D11Resource*)dst, (ID3D11Resource*)src);
}

void Renderer::CopyCPUToGPU(void* data, size_t size, UnorderedAccessView* uav)
{
	// add size check later
	D3D11_MAPPED_SUBRESOURCE mappedSubresource;

	HRESULT hr = m_deviceContext->Map(uav->m_buffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &mappedSubresource);
	if (!SUCCEEDED(hr)) {
		ERROR_AND_DIE("Could not map content.");
	}

	memcpy(mappedSubresource.pData, data, size);
	m_deviceContext->Unmap(uav->m_buffer, NULL);
}

void Renderer::CopyGPUToCPU(void* data, size_t size, ID3D11Buffer* outBuffer)
{
	D3D11_MAPPED_SUBRESOURCE mappedSubresource;

	HRESULT hr = m_deviceContext->Map(outBuffer, NULL, D3D11_MAP_READ, NULL, &mappedSubresource);
	if (!SUCCEEDED(hr)) {
		ERROR_AND_DIE("Could not map content.");
	}

	memcpy(data, mappedSubresource.pData, size);
	m_deviceContext->Unmap(outBuffer, NULL);
}

void Renderer::CopyCPUToGPU(void* data, size_t size, ID3D11Buffer* inBuffer)
{
	D3D11_MAPPED_SUBRESOURCE mappedSubresource;

	HRESULT hr = m_deviceContext->Map(inBuffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &mappedSubresource);
	if (!SUCCEEDED(hr)) {
		ERROR_AND_DIE("Could not map content.");
	}

	memcpy(mappedSubresource.pData, data, size);
	m_deviceContext->Unmap(inBuffer, NULL);
}


void Renderer::BindVertexBuffer(VertexBuffer* vbo, Topology topology)
{
	UINT strides = vbo->GetStride();
	UINT offsets = 0;

	m_deviceContext->IASetVertexBuffers(NULL, 1, &vbo->m_buffer, &strides, &offsets);
	
	
	m_deviceContext->IASetPrimitiveTopology(GetPrimitiveTopology(topology));
}

void Renderer::BindIndexBuffer(IndexBuffer* ibo)
{
	m_deviceContext->IASetIndexBuffer(ibo->m_buffer, DXGI_FORMAT_R32_UINT, 0);
}

ConstantBuffer* Renderer::CreateConstantBuffer(const size_t size)
{
	ConstantBuffer* newConstantBuffer = new ConstantBuffer(size);

	D3D11_BUFFER_DESC bufferDesc = { 0 };
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.ByteWidth = (UINT)size;
	bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	HRESULT	hr = m_device->CreateBuffer(&bufferDesc, NULL, &newConstantBuffer->m_buffer);
	if (!SUCCEEDED(hr)) {
		ERROR_AND_DIE("Could not create constant buffer.");
	}
	std::string debugName = Stringf("Constant Buffer - size: %d", size);
	SetObjDebugName(newConstantBuffer->m_buffer, debugName.c_str());
	return newConstantBuffer;
}

void Renderer::CopyCPUToGPU(const void* data, size_t size, ConstantBuffer*& cbo)
{
	D3D11_MAPPED_SUBRESOURCE mappedSubresource;

	HRESULT hr = m_deviceContext->Map(cbo->m_buffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &mappedSubresource);
	if (!SUCCEEDED(hr)) {
		ERROR_AND_DIE("Could not map content.");
	}

	memcpy(mappedSubresource.pData, data, size);
	m_deviceContext->Unmap(cbo->m_buffer, NULL);
}

void Renderer::BindConstantBuffer(int slot, ConstantBuffer* cbo)
{
	m_deviceContext->VSSetConstantBuffers(slot, 1, &cbo->m_buffer);

	m_deviceContext->GSSetConstantBuffers(slot, 1, &cbo->m_buffer);

	m_deviceContext->PSSetConstantBuffers(slot, 1, &cbo->m_buffer);

	//m_deviceContext->CSSetConstantBuffers(slot, 1, &cbo->m_buffer);

	//m_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void Renderer::VSBindConstantBuffer(int slot, ConstantBuffer* cbo)
{
	m_deviceContext->VSSetConstantBuffers(slot, 1, &cbo->m_buffer);
}

void Renderer::PSBindConstantBuffer(int slot, ConstantBuffer* cbo)
{
	m_deviceContext->PSSetConstantBuffers(slot, 1, &cbo->m_buffer);
}

void Renderer::GSBindConstantBuffer(int slot, ConstantBuffer* cbo)
{
	m_deviceContext->GSSetConstantBuffers(slot, 1, &cbo->m_buffer);
}

void Renderer::CSBindConstantBuffer(int slot, ConstantBuffer* cbo)
{
	m_deviceContext->CSSetConstantBuffers(slot, 1, &cbo->m_buffer);
}

Texture* Renderer::GetTextureForFileName(char const* imageFilePath)
{
	for (int textureIndex = 0; textureIndex < m_loadedTextures.size(); textureIndex++)
	{
		if (m_loadedTextures[textureIndex]->m_name == imageFilePath)
		{
			return m_loadedTextures[textureIndex];
		}
	}
	return nullptr;
}

Texture* Renderer::CreateTextureFromData(char const* name, IntVec2 dimensions, int bytesPerTexel, uint8_t* texelData)
{
	// Check if the load was successful
	GUARANTEE_OR_DIE(texelData, Stringf("CreateTextureFromData failed for \"%s\" - texelData was null!", name));
	GUARANTEE_OR_DIE(bytesPerTexel >= 3 && bytesPerTexel <= 4, Stringf("CreateTextureFromData failed for \"%s\" - unsupported BPP=%i (must be 3 or 4)", name, bytesPerTexel));
	GUARANTEE_OR_DIE(dimensions.x > 0 && dimensions.y > 0, Stringf("CreateTextureFromData failed for \"%s\" - illegal texture dimensions (%i x %i)", name, dimensions.x, dimensions.y));

	Texture* newTexture = new Texture();
	newTexture->m_name = name; // NOTE: m_name must be a std::string, otherwise it may point to temporary data!
	newTexture->m_dimensions = dimensions;

	m_loadedTextures.push_back(newTexture);
	return newTexture;
}

Texture* Renderer::CreateTextureFromImage(const Image& image)
{
	TextureConfig config;
	config.m_name = image.GetImageFilePath();
	config.m_width = image.GetDimensions().x;
	config.m_height = image.GetDimensions().y;
	config.m_format = ResourceFormat::R8G8B8A8_UNORM;
	config.m_bindFlags = RESOURCE_BINDFLAGS_SHADER_RESOURCE_VIEW;
	config.m_initialData = (void*)image.GetRawData();
	config.m_stride = image.GetDimensions().x * 4;
	return CreateTextureByConfig(config);

}

Texture* Renderer::CreateCubemapTextureFromFiles(Strings files)
{
	if (files.size() < 6) {
		ERROR_AND_DIE(Stringf("Can't create cube map texture"));
	}

	// See if we already have this texture previously loaded
	Texture* existingTexture = GetTextureForFileName(files[0].c_str());
	if (existingTexture)
	{
		return existingTexture;
	}

	// Never seen this texture before!  Let's load it.
	TextureConfig config;

	config.m_format = ResourceFormat::R8G8B8A8_UNORM;
	config.m_bindFlags = RESOURCE_BINDFLAGS_SHADER_RESOURCE_VIEW;
	config.m_initialData = nullptr;
	config.m_files = files;
	config.m_type = TextureType::CUBEMAP;

	Texture* newTexture = CreateTextureByConfig(config);
	return newTexture;
}

Texture* Renderer::CreateTextureByConfig(TextureConfig config)
{
	Texture* newTexture = new Texture();
	D3D11_TEXTURE2D_DESC textureDesc = { 0 };
	D3D11_SUBRESOURCE_DATA pIntialData{};
	D3D11_SUBRESOURCE_DATA data[6];
	D3D11_SUBRESOURCE_DATA* pInitialDataPtr = nullptr;
	HRESULT hr = S_OK;
	if (config.m_type == TextureType::DEFAULT) {
		textureDesc.Width = config.m_width;
		textureDesc.Height = config.m_height;
		textureDesc.MipLevels = 1;
		textureDesc.ArraySize = 1;
		textureDesc.Format = ResourceView::GetDX11FormatFromEnum(config.m_format);
		textureDesc.Usage = ResourceView::GetDX11UsageFromMemoryUsage(config.m_usage);
		textureDesc.BindFlags = ResourceView::GetDX11BindflagsFromUINT(config.m_bindFlags);
		textureDesc.SampleDesc.Count = 1;

		if (config.m_initialData != nullptr) {
			pIntialData.pSysMem = config.m_initialData;
			pIntialData.SysMemPitch = config.m_stride;
			pInitialDataPtr = &pIntialData;

		}
		hr = m_device->CreateTexture2D(&textureDesc, pInitialDataPtr, &newTexture->m_texture2);
	}
	else if (config.m_type == TextureType::CUBEMAP){
		textureDesc.MipLevels = 1;
		textureDesc.ArraySize = 6;
		textureDesc.CPUAccessFlags = 0;
		textureDesc.MiscFlags = D3D11_RESOURCE_MISC_TEXTURECUBE;
		Image image[6];
		for (int filesIndex = 0; filesIndex < 6; filesIndex++) {
			image[filesIndex] = Image(config.m_files[filesIndex].c_str());
		}
		config.m_width = image[0].GetDimensions().x;
		config.m_height = image[0].GetDimensions().y;
		config.m_stride = image[0].GetDimensions().x * 4;
		config.m_name = image[0].GetImageFilePath();

		textureDesc.Width = config.m_width;
		textureDesc.Height = config.m_height;
		textureDesc.Format = ResourceView::GetDX11FormatFromEnum(config.m_format);
		textureDesc.Usage = ResourceView::GetDX11UsageFromMemoryUsage(config.m_usage);
		textureDesc.BindFlags = ResourceView::GetDX11BindflagsFromUINT(config.m_bindFlags);
		textureDesc.SampleDesc.Count = 1;

		for (int i = 0; i < 6; i++) {
			data[i].pSysMem = image[i].GetRawData();
			data[i].SysMemPitch = image[i].GetDimensions().x * 4;
			data[i].SysMemSlicePitch = 0;
		}
		hr = m_device->CreateTexture2D(&textureDesc, data, &newTexture->m_texture2);
	}

	if (!SUCCEEDED(hr)) {
		ERROR_AND_DIE(Stringf("Could not create Texture %s.", config.m_name.c_str()));
	}
	newTexture->m_config = config;
	newTexture->m_dimensions = IntVec2(config.m_width, config.m_height);
	newTexture->m_owner = this;
	newTexture->m_name = config.m_name;
	m_loadedTextures.push_back(newTexture);
	return newTexture;
}

StructuredBuffer* Renderer::CreateStructuredBufferByData(size_t numOfElements, size_t stride, void* data)
{
	BufferConfig bufferConfig;
	bufferConfig.m_numElements = numOfElements;
	bufferConfig.m_stride = stride;
	bufferConfig.m_data = data;
	return CreateStructuredBufferByConfig(bufferConfig);
}

StructuredBuffer* Renderer::CreateStructuredBufferByConfig(BufferConfig config)
{
	StructuredBuffer* buffer = new StructuredBuffer();
	buffer->m_config = config;

	D3D11_BUFFER_DESC descBuffer = {};
	descBuffer.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	descBuffer.ByteWidth = UINT(config.m_numElements * config.m_stride);
	descBuffer.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

	descBuffer.StructureByteStride = (UINT)config.m_stride;

	D3D11_SUBRESOURCE_DATA* initialData = nullptr;

	if (config.m_data) {
		initialData = new D3D11_SUBRESOURCE_DATA();
		initialData->pSysMem = config.m_data;
		initialData->SysMemPitch = UINT(config.m_numElements * config.m_stride);
	}

	HRESULT bufferCreation = m_device->CreateBuffer(&descBuffer, initialData, &buffer->m_buffer);
	if (!SUCCEEDED(bufferCreation)) {
		ERROR_AND_DIE("Error: Couldn't create structured buffer");
	}
	// get a reference of owner
	buffer->m_owner = this;

	return buffer;
}

void Renderer::BindTexture(Texture* texture, int slot)
{
	if (texture == nullptr) {
		if (slot == 0)
			texture = (Texture*)m_defaultTexture;
		else {
			ID3D11ShaderResourceView* ppSRViewNULL[1] = { NULL };
			m_deviceContext->PSSetShaderResources(slot, 1, ppSRViewNULL);
			//m_deviceContext->VSSetShaderResources()
			return;
		}
	}
	m_deviceContext->PSSetShaderResources(slot, 1, &texture->GetOrCreateResourceViewByFlag(RESOURCE_BINDFLAGS_SHADER_RESOURCE_VIEW)->m_SRV);
}


void Renderer::BindBackBufferAsTexture(bool isBinding /*= true*/, int slot /*= 0*/)
{
	if (isBinding) {
		m_deviceContext->PSSetShaderResources(slot, 1, &m_preBackBuffer->GetResourceViewByFlag(RESOURCE_BINDFLAGS_SHADER_RESOURCE_VIEW)->m_SRV);
	}
	else {
		ID3D11ShaderResourceView* ppSRViewNULL[1] = { NULL };
		m_deviceContext->PSSetShaderResources(slot, 1, ppSRViewNULL);
	}
	return;
}

void Renderer::BindTextures(Texture* texture0, Texture* texture1, Texture* texture2)
{
	if (texture0) {
		m_deviceContext->PSSetShaderResources(0, 1, &texture0->GetOrCreateResourceViewByFlag(RESOURCE_BINDFLAGS_SHADER_RESOURCE_VIEW)->m_SRV);
	}
	else {
		ID3D11ShaderResourceView* ppSRViewNULL[1] = { NULL };
		m_deviceContext->PSSetShaderResources(0, 1, ppSRViewNULL);
	}
	if (texture1) {
		m_deviceContext->PSSetShaderResources(1, 1, &texture1->GetOrCreateResourceViewByFlag(RESOURCE_BINDFLAGS_SHADER_RESOURCE_VIEW)->m_SRV);
	}
	else {
		ID3D11ShaderResourceView* ppSRViewNULL[1] = { NULL };
		m_deviceContext->PSSetShaderResources(1, 1, ppSRViewNULL);
	}
	if (texture2) {
		m_deviceContext->PSSetShaderResources(2, 1, &texture2->GetOrCreateResourceViewByFlag(RESOURCE_BINDFLAGS_SHADER_RESOURCE_VIEW)->m_SRV);
	}
	else {
		ID3D11ShaderResourceView* ppSRViewNULL[1] = { NULL };
		m_deviceContext->PSSetShaderResources(2, 1, ppSRViewNULL);
	}
}

void Renderer::BindShader(Shader* shader)
{
	if (shader == nullptr) {
		m_currentShader = m_defaultShader;
	}
	else {
		m_currentShader = shader;
	}
	//m_deviceContext->Vss
	if (m_currentShader->m_vertexShader) {
		m_deviceContext->VSSetShader(m_currentShader->m_vertexShader, NULL, NULL);
		m_deviceContext->IASetInputLayout(m_currentShader->m_inputLayout);
	}
	if (m_currentShader->m_pixelShader) {
		m_deviceContext->PSSetShader(m_currentShader->m_pixelShader, NULL, NULL);
	}
	else {
		m_deviceContext->PSSetShader(NULL, NULL, NULL);
	}
	if (m_currentShader->m_geometryShader) {
		m_deviceContext->GSSetShader(m_currentShader->m_geometryShader, NULL, NULL);
	}
	else {
		m_deviceContext->GSSetShader(NULL, NULL, NULL);
	}
	if (m_currentShader->m_computeShader) {
		m_deviceContext->CSSetShader(m_currentShader->m_computeShader, NULL, NULL);
	}
}

void Renderer::IASetVertexBuffers(VertexBuffer* vbo)
{
	if (vbo == nullptr) {
		ID3D11Buffer* pNullBuffer = nullptr;
		UINT          iNullUINT = 0;
		m_deviceContext->IASetVertexBuffers(0, 1, &pNullBuffer, &iNullUINT, &iNullUINT);
		return;
	}
	UINT strides = vbo->GetStride();
	UINT offsets = 0;

	m_deviceContext->IASetVertexBuffers(NULL, 1, &vbo->m_buffer, &strides, &offsets);
}

void Renderer::IASetPrimitiveTopologyToPointList()
{
	m_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
}

void Renderer::IASetPrimitiveTopology(Topology topology)
{
	switch (topology)
	{
	case Topology::LineList: {
		m_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);
		return;
	}
	case Topology::TriangleList: {
		m_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		return;
	}
	case Topology::PointList: {
		m_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);
		return;
	}
	default:
	{
		m_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		return;
	}
	}
}

void Renderer::IASetInputLayout(Shader* shader)
{
	if (shader == nullptr)
		shader = m_defaultShader;
	m_deviceContext->IASetInputLayout(shader->m_inputLayout);
}

void Renderer::VSSetShader(Shader* shader)
{
	if (shader == nullptr)
		shader = m_defaultShader;
	if (shader->m_vertexShader) {
		m_deviceContext->VSSetShader(shader->m_vertexShader, nullptr, 0);
	}
	else
	{
		m_deviceContext->VSSetShader(nullptr, nullptr, 0);
	}
}

void Renderer::GSSetShader(Shader* shader)
{
	if (shader == nullptr)
		shader = m_defaultShader;
	if (shader->m_geometryShader) {
		m_deviceContext->GSSetShader(shader->m_geometryShader, nullptr, 0);
	}
	else
	{
		m_deviceContext->GSSetShader(nullptr, nullptr, 0);
	}
}

void Renderer::PSSetShader(Shader* shader)
{
	if (shader == nullptr)
		shader = m_defaultShader;
	if (shader->m_pixelShader) {
		m_deviceContext->PSSetShader(shader->m_pixelShader, nullptr, 0);
	}
	else
	{
		m_deviceContext->PSSetShader(nullptr, nullptr, 0);
	}
}

void Renderer::BindDepthStencil()
{
	m_deviceContext->OMSetRenderTargets(1, &m_preBackBuffer->GetResourceViewByFlag(RESOURCE_BINDFLAGS_RENDER_TARGET_VIEW)->m_RTV, m_depthStencilTexture->GetOrCreateResourceViewByFlag(RESOURCE_BINDFLAGS_DEPTH_STENCIL_VIEW)->m_DSV);
}

void Renderer::UnbindDepthStencil(Texture const* renderTarget)
{
	if (renderTarget == nullptr)
		m_deviceContext->OMSetRenderTargets(1, &m_preBackBuffer->GetResourceViewByFlag(RESOURCE_BINDFLAGS_RENDER_TARGET_VIEW)->m_RTV, NULL);
	else m_deviceContext->OMSetRenderTargets(1, &renderTarget->GetResourceViewByFlag(RESOURCE_BINDFLAGS_RENDER_TARGET_VIEW)->m_RTV, NULL);
}

void Renderer::CSSetShader(Shader* shader)
{
	if (shader && shader->m_computeShader) {
		m_deviceContext->CSSetShader(shader->m_computeShader, nullptr, 0);
	} 
	else {
		m_deviceContext->CSSetShader(nullptr, nullptr, 0);
	}
}

void Renderer::SetDepthTexture(Texture const* texture, Texture const* renderTarget)
{
	if (texture == nullptr) {
		if (renderTarget == nullptr)
			m_deviceContext->OMSetRenderTargets(1, &m_preBackBuffer->GetResourceViewByFlag(RESOURCE_BINDFLAGS_RENDER_TARGET_VIEW)->m_RTV, m_depthStencilTexture->GetOrCreateResourceViewByFlag(RESOURCE_BINDFLAGS_DEPTH_STENCIL_VIEW)->m_DSV);
		else m_deviceContext->OMSetRenderTargets(1, &renderTarget->GetResourceViewByFlag(RESOURCE_BINDFLAGS_RENDER_TARGET_VIEW)->m_RTV, m_depthStencilTexture->GetOrCreateResourceViewByFlag(RESOURCE_BINDFLAGS_DEPTH_STENCIL_VIEW)->m_DSV);
	}
	else {
		if (renderTarget == nullptr)
		m_deviceContext->OMSetRenderTargets(1, &m_preBackBuffer->GetResourceViewByFlag(RESOURCE_BINDFLAGS_RENDER_TARGET_VIEW)->m_RTV, texture->GetResourceViewByFlag(RESOURCE_BINDFLAGS_DEPTH_STENCIL_VIEW)->m_DSV);
		else m_deviceContext->OMSetRenderTargets(1, &renderTarget->GetResourceViewByFlag(RESOURCE_BINDFLAGS_RENDER_TARGET_VIEW)->m_RTV, texture->GetResourceViewByFlag(RESOURCE_BINDFLAGS_DEPTH_STENCIL_VIEW)->m_DSV);
	}
}

Shader* Renderer::CreateShader(char const* shaderName, char const* shaderSource)
{
	ShaderConfig config;
	config.m_name = shaderName;
	Shader* newShader = new Shader(config);

	std::string s_shaderSource = shaderSource;

	// compile the vertex shader
	std::vector<unsigned char> vertexShader;
	CompileShaderToByteCode(vertexShader, newShader->GetName().c_str(), s_shaderSource.c_str(), config.m_vertexEntryPoint.c_str(), "vs_5_0");

	// create the vertex shader
	HRESULT hr = m_device->CreateVertexShader(vertexShader.data(), vertexShader.size(), NULL, &(newShader->m_vertexShader));
	if (!SUCCEEDED(hr)) {
		ERROR_AND_DIE("Could not create the vertex shader.");
	}
	

	// compile the pixel shader
	std::vector<unsigned char> pixelShader;
	CompileShaderToByteCode(pixelShader, newShader->GetName().c_str(), s_shaderSource.c_str(), config.m_pixelEntryPoint.c_str(), "ps_5_0");

	// create pixel shader
	hr = m_device->CreatePixelShader(pixelShader.data(), pixelShader.size(), NULL, &(newShader->m_pixelShader));
	if (!SUCCEEDED(hr)) {
		ERROR_AND_DIE("Could not create the pixel shader.");
	}

	// create input layout	
	CreateInputLayoutFromVS(vertexShader, &newShader->m_inputLayout);

	m_loadedShaders.push_back(newShader);
	return newShader;
}

Shader* Renderer::CreateShader(char const* shaderName)
{
	std::string fullPath = shaderName;
	fullPath.append(".hlsl");
	std::string fileText;
	FileReadToString(fileText, fullPath.c_str());
	return CreateShader(shaderName, fileText.c_str());
}

Shader* Renderer::CreateShaderByConfig(ShaderConfig config)
{
	Shader* newShader = new Shader(config);
	std::string fullPath = config.m_name;
	fullPath.append(".hlsl");

	std::string fileText;
	FileReadToString(fileText, fullPath.c_str());

	std::string s_shaderSource = fileText;

	if (config.m_hasVS) {
		// compile the vertex shader
		std::vector<unsigned char> vertexShader;
		CompileShaderToByteCode(vertexShader, newShader->GetName().c_str(), s_shaderSource.c_str(), config.m_vertexEntryPoint.c_str(), "vs_5_0");

		// create the vertex shader
		HRESULT hr = m_device->CreateVertexShader(vertexShader.data(), vertexShader.size(), NULL, &(newShader->m_vertexShader));
		if (!SUCCEEDED(hr)) {
			ERROR_AND_DIE("Could not create the vertex shader.");
		}
		std::string debugName = "VS | " + newShader->GetName();
		SetObjDebugName(newShader->m_vertexShader, debugName.c_str());
		// create input layout	
		CreateInputLayoutFromVS(vertexShader, &newShader->m_inputLayout);
		debugName = "Input Layout | " + newShader->GetName();
		SetObjDebugName(newShader->m_inputLayout, debugName.c_str());
	}

	if (config.m_hasGS) {
		std::vector<unsigned char> geometryShader;
		CompileShaderToByteCode(geometryShader, newShader->GetName().c_str(), s_shaderSource.c_str(), config.m_geometryEntryPoint.c_str(), "gs_5_0");

		// create geometry shader

		HRESULT hr = m_device->CreateGeometryShader(geometryShader.data(), geometryShader.size(), NULL, &(newShader->m_geometryShader));
		if (!SUCCEEDED(hr)) {
			ERROR_AND_DIE("Could not create the geometry shader.");
		}
		std::string debugName = "GS | " + newShader->GetName();
		SetObjDebugName(newShader->m_geometryShader, debugName.c_str());
	}

	if (config.m_hasPS) {
		// compile the pixel shader
		std::vector<unsigned char> pixelShader;
		CompileShaderToByteCode(pixelShader, newShader->GetName().c_str(), s_shaderSource.c_str(), config.m_pixelEntryPoint.c_str(), "ps_5_0");

		// create pixel shader
		HRESULT hr = m_device->CreatePixelShader(pixelShader.data(), pixelShader.size(), NULL, &(newShader->m_pixelShader));
		if (!SUCCEEDED(hr)) {
			ERROR_AND_DIE("Could not create the pixel shader.");
		}
		std::string debugName = "PS | " + newShader->GetName();
		SetObjDebugName(newShader->m_pixelShader, debugName.c_str());
	}
	if (config.m_hasCS) {
		std::filesystem::path shaderName = config.m_name;
		std::filesystem::path shaderObjName = config.m_name + config.m_computeEntryPoint;
		//shaderName += config.m_computeEntryPoint;
		//std::string newFileName = shaderName.filename().string() + config.m_computeEntryPoint;
		//shaderName.replace_filename(config.m_name + config.m_computeEntryPoint);
		//shaderName.filename() += config.m_computeEntryPoint;
		std::string shaderFileName = shaderObjName.filename().replace_extension("cso").string();
		std::string compileObjectName = "Data/CompiledShaders/" + shaderFileName;

		std::string filename = shaderName.replace_extension(".hlsl").string();
		std::string shaderSource;

		FileReadToString(shaderSource, filename.c_str());

		std::string debugName = "CS | " + shaderName.string() + " | " + config.m_computeEntryPoint;

		std::vector<uint8_t> CSShaderByteCode;

		//if cso is already compiled
		if (IsFileExist(compileObjectName)) {
			FileReadToBuffer(CSShaderByteCode, compileObjectName);
		}
		else {
			CompileShaderToByteCode(CSShaderByteCode, shaderName.string().c_str(), shaderSource.c_str(), config.m_computeEntryPoint.c_str(), "cs_5_0");
			WriteToFile(CSShaderByteCode, compileObjectName);
		}
		m_device->CreateComputeShader(CSShaderByteCode.data(), CSShaderByteCode.size(), NULL, &newShader->m_computeShader);
		SetObjDebugName(newShader->m_computeShader, debugName.c_str());
	}

	m_loadedShaders.push_back(newShader);
	return newShader;
}

Shader* Renderer::CreateComputeShader(std::filesystem::path shaderName)
{
	std::string shaderFileName = shaderName.filename().replace_extension("cso").string();
	std::string compileObjectName = "Data/CompiledShaders/" + shaderFileName;

	std::string filename = shaderName.replace_extension(".hlsl").string();
	std::string shaderSource;

	FileReadToString(shaderSource, filename.c_str());

	ShaderConfig config;
	config.m_name = shaderName.string();
	Shader* newShader = new Shader(config);
	std::string debugName = "CS | " + shaderName.string();

	std::vector<uint8_t> CSShaderByteCode;
	
	//if cso is already compiled
	if (IsFileExist(compileObjectName)) {
		FileReadToBuffer(CSShaderByteCode, compileObjectName);
	}
	else {
		CompileShaderToByteCode(CSShaderByteCode, shaderName.string().c_str(), shaderSource.c_str(), config.m_computeEntryPoint.c_str(), "cs_5_0");
		WriteToFile(CSShaderByteCode, compileObjectName);
	}
	m_device->CreateComputeShader(CSShaderByteCode.data(), CSShaderByteCode.size(), NULL, &newShader->m_computeShader);
	SetObjDebugName(newShader->m_computeShader, debugName.c_str());
	m_loadedShaders.push_back(newShader);
	return newShader;
}

bool Renderer::CompileShaderToByteCode(std::vector<unsigned char>& outByteCode, char const* name, char const* source, char const* entryPoint, char const* target)
{
	UNUSED(name);
	ID3DBlob* shaderBlob = nullptr;
	ID3DBlob* errorBlob = nullptr;

	DWORD compileFlags = D3DCOMPILE_OPTIMIZATION_LEVEL3;
#if defined(ENGINE_DEBUG_RENDER)
	compileFlags = D3DCOMPILE_DEBUG;
	compileFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
	compileFlags |= D3DCOMPILE_WARNINGS_ARE_ERRORS;
#endif

	HRESULT hr = D3DCompile(source, strlen(source), name, NULL, D3D_COMPILE_STANDARD_FILE_INCLUDE, entryPoint, target, compileFlags, NULL, &shaderBlob, &errorBlob);
	if (!SUCCEEDED(hr)) {
		DebuggerPrintf((const char*)errorBlob->GetBufferPointer());
		DX_SAFE_RELEASE(shaderBlob);
		DX_SAFE_RELEASE(errorBlob);
		ERROR_AND_DIE("Could not compile the shader.");
	}
	shaderBlob->GetBufferPointer();
	outByteCode.resize(shaderBlob->GetBufferSize());
	memcpy(outByteCode.data(), shaderBlob->GetBufferPointer(), shaderBlob->GetBufferSize());

	DX_SAFE_RELEASE(shaderBlob);
	DX_SAFE_RELEASE(errorBlob);
	return true;
}

void Renderer::CreateBlendStates()
{
	D3D11_BLEND_DESC blendDesc[(int)(BlendMode::COUNT)] = { 0 };

	//blendDesc.RenderTarget[0].BlendEnable = true;
	blendDesc[(int)(BlendMode::OPAQUE)].RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	blendDesc[(int)(BlendMode::OPAQUE)].RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;

	blendDesc[(int)(BlendMode::ADDITIVE)].RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendDesc[(int)(BlendMode::ADDITIVE)].RenderTarget[0].DestBlend = D3D11_BLEND_ONE;

	blendDesc[(int)(BlendMode::ALPHA)].RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	blendDesc[(int)(BlendMode::ALPHA)].RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;

	for (int i = 0; i < (int)(BlendMode::COUNT); i++) {
		blendDesc[i].RenderTarget[0].BlendEnable = true;
		blendDesc[i].RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
		blendDesc[i].RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
		blendDesc[i].RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ONE;
		blendDesc[i].RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		blendDesc[i].RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
		HRESULT hr = m_device->CreateBlendState(&blendDesc[i], &m_blendStates[i]);
		if (!SUCCEEDED(hr)) {
			ERROR_AND_DIE("Could not create blend state." + i);
		}
	}
}

void Renderer::CreateSamplerStates()
{
	D3D11_SAMPLER_DESC samplerDesc = { };
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	HRESULT hr = m_device->CreateSamplerState(&samplerDesc, &m_samplerStates[(int)SamplerMode::POINT_CLAMP]);
	if (!SUCCEEDED(hr)) {
		ERROR_AND_DIE("Could not create point clamp sampler state.");
	}
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

	hr = m_device->CreateSamplerState(&samplerDesc, &m_samplerStates[(int)SamplerMode::BILINEAR_WRAP]);
	if (!SUCCEEDED(hr)) {
		ERROR_AND_DIE("Could not create bilinear wrap sampler state.");
	}
}

void Renderer::CreateRasterizerStates()
{
	// create rasterizer modes
	D3D11_RASTERIZER_DESC rasterizerDesc[(int)RasterizerMode::COUNT] = {};
	for (int i = 0; i < (int)RasterizerMode::COUNT; i++) {
		//rasterizerDesc[i].FillMode = D3D11_FILL_SOLID;
		rasterizerDesc[i].FrontCounterClockwise = true;
		rasterizerDesc[i].DepthClipEnable = true;
		rasterizerDesc[i].AntialiasedLineEnable = true;
	}
	rasterizerDesc[(int)RasterizerMode::SOLID_CULL_BACK].FillMode = D3D11_FILL_SOLID;
	rasterizerDesc[(int)RasterizerMode::SOLID_CULL_NONE].FillMode = D3D11_FILL_SOLID;
	rasterizerDesc[(int)RasterizerMode::SOLID_CULL_BACK].CullMode = D3D11_CULL_BACK;
	rasterizerDesc[(int)RasterizerMode::SOLID_CULL_NONE].CullMode = D3D11_CULL_NONE;
	rasterizerDesc[(int)RasterizerMode::WIREFRAME_CULL_BACK].FillMode = D3D11_FILL_WIREFRAME;
	rasterizerDesc[(int)RasterizerMode::WIREFRAME_CULL_NONE].FillMode = D3D11_FILL_WIREFRAME;
	rasterizerDesc[(int)RasterizerMode::WIREFRAME_CULL_BACK].CullMode = D3D11_CULL_BACK;
	rasterizerDesc[(int)RasterizerMode::WIREFRAME_CULL_NONE].CullMode = D3D11_CULL_BACK;

	HRESULT hr;
	//ID3D11RasterizerState* rasterizerState = nullptr;
	for (int i = 0; i < (int)RasterizerMode::COUNT; i++) {
		hr = m_device->CreateRasterizerState(&rasterizerDesc[i], &m_rasterizerStates[i]);
		if (!SUCCEEDED(hr)) {
			ERROR_AND_DIE("Could not create rasterizer state." + i);
		}
	}
}

void Renderer::CreateDepthStates()
{
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc = { };
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS_EQUAL;

	HRESULT hr = m_device->CreateDepthStencilState(&depthStencilDesc, &m_depthStencilStates[(int)DepthMode::ENABLED]);

	if (!SUCCEEDED(hr)) {
		ERROR_AND_DIE("Could not create enable depth stencil state.");
	}

	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ZERO;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_ALWAYS;

	hr = m_device->CreateDepthStencilState(&depthStencilDesc, &m_depthStencilStates[(int)DepthMode::DISABLED]);

	if (!SUCCEEDED(hr)) {
		ERROR_AND_DIE("Could not create disable depth stencil state.");
	}

}

BitmapFont* Renderer::CreateOrGetBitmapFont(const char* bitmapFontFilePathWithNoExtension)
{
	// See if we already have this texture previously loaded
	BitmapFont* existingBitmapFont = GetBitmapFontForFileName(bitmapFontFilePathWithNoExtension);
	if (existingBitmapFont)
	{
		return existingBitmapFont;
	}

	// Never seen this texture before!  Let's load it.
	BitmapFont* newBitmapFont = CreateBitmapFont(bitmapFontFilePathWithNoExtension);
	return newBitmapFont;
}

BitmapFont* Renderer::GetBitmapFontForFileName(const char* bitmapFontFilePathWithNoExtension)
{
	for (int fontIndex = 0; fontIndex < m_loadedFonts.size(); fontIndex++)
	{
		if (m_loadedFonts[fontIndex]->m_fontFilePathNameWithNoExtension == bitmapFontFilePathWithNoExtension)
		{
			return m_loadedFonts[fontIndex];
		}
	}
	return nullptr;
}

BitmapFont* Renderer::CreateBitmapFont(const char* bitmapFontFilePathWithNoExtension)
{
	std::string fullPath = bitmapFontFilePathWithNoExtension;
	fullPath.append(".png");
	Texture* texture = CreateOrGetTextureFromFile(fullPath.c_str());
	BitmapFont* newBitmapFont = new BitmapFont(bitmapFontFilePathWithNoExtension, *texture);
	return newBitmapFont;
}

ResourceView* Renderer::GetOrCreateView(ResourceViewConfig config, ID3D11Buffer* resource)
{
	RESOURCE_BINDFLAGS flag = config.m_flag;
	ResourceView* view = new ResourceView(config, resource);
	switch (flag)
	{
	case RESOURCE_BINDFLAGS_SHADER_RESOURCE_VIEW:
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
		srvDesc.BufferEx.FirstElement = 0;
		srvDesc.BufferEx.NumElements = (UINT)config.m_numOfElements;
		srvDesc.Format = ResourceView::GetDX11FormatFromEnum(config.m_format);
		HRESULT hr = m_device->CreateShaderResourceView(resource, &srvDesc, &view->m_SRV);
		if (!SUCCEEDED(hr)) {
			ERROR_AND_DIE("Can't create SRV of buffer resource");
		}
		std::string debugName = Stringf("Structured Buffer - SRV : numOfElements:%d", config.m_numOfElements);
		SetObjDebugName(view->m_SRV, debugName.c_str());
		break;
	}
	case RESOURCE_BINDFLAGS_UNORDERED_ACCESS_VIEW:
	{
		D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
		uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
		uavDesc.Format = ResourceView::GetDX11FormatFromEnum(config.m_format);    
		uavDesc.Buffer.FirstElement = 0;
		uavDesc.Buffer.NumElements = (UINT)config.m_numOfElements;
		HRESULT hr = m_device->CreateUnorderedAccessView(resource, &uavDesc, &view->m_UAV);
		if (!SUCCEEDED(hr)) {
			ERROR_AND_DIE("Can't create UAV of buffer resource");
		}
		std::string debugName = Stringf("Structured Buffer - UAV : numOfElements:%d", config.m_numOfElements);
		SetObjDebugName(view->m_UAV, debugName.c_str());
		break;
	}
	case RESOURCE_BINDFLAGS_RENDER_TARGET_VIEW:
	{
		break;
	}
	case RESOURCE_BINDFLAGS_DEPTH_STENCIL_VIEW:
	{
		break;
	}
	default:
		break;
	}
	return view;
}

ResourceView* Renderer::GetOrCreateView(ResourceViewConfig config, ID3D11Resource* resource)
{
	RESOURCE_BINDFLAGS flag = config.m_flag;
	ResourceView* view = new ResourceView(config, resource);
	switch (flag)
	{
	case RESOURCE_BINDFLAGS_SHADER_RESOURCE_VIEW:
	{
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
		if (config.m_viewDimension == ViewDimension::TEXTURECUBE)
			srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURECUBE;
		srvDesc.Format = ResourceView::GetDX11FormatColorFromEnum(config.m_format);
		srvDesc.Texture2D.MipLevels = 1;
		srvDesc.Texture2D.MostDetailedMip = 0;

		HRESULT hr = m_device->CreateShaderResourceView(resource, &srvDesc, &view->m_SRV);
		if (!SUCCEEDED(hr)) {
			ERROR_AND_DIE("Can't create SRV of texture resource");
		}
		std::string debugName = Stringf("Texture - SRV : format:%d", config.m_format);
		SetObjDebugName(view->m_SRV, debugName.c_str());
		break;
	}
	case RESOURCE_BINDFLAGS_UNORDERED_ACCESS_VIEW:
	{
		D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
		uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
		uavDesc.Format = ResourceView::GetDX11FormatFromEnum(config.m_format);

		HRESULT hr = m_device->CreateUnorderedAccessView(resource, &uavDesc, &view->m_UAV);
		if (!SUCCEEDED(hr)) {
			ERROR_AND_DIE("Can't create UAV of texture resource");
		}
		std::string debugName = Stringf("Texture - UAV", config.m_numOfElements);
		SetObjDebugName(view->m_UAV, debugName.c_str());
		break;
	}
	case RESOURCE_BINDFLAGS_RENDER_TARGET_VIEW:
	{
		D3D11_RENDER_TARGET_VIEW_DESC desc = {};
		desc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;

		HRESULT hr = m_device->CreateRenderTargetView(resource, &desc, &view->m_RTV);
		if (!SUCCEEDED(hr)) {
			ERROR_AND_DIE("Can't create RTV of texture resource");
		}
		std::string debugName = Stringf("Texture - RTV : fomat:%d", config.m_format);
		SetObjDebugName(view->m_RTV, debugName.c_str());
		break;
	}
	case RESOURCE_BINDFLAGS_DEPTH_STENCIL_VIEW:
	{
		D3D11_DEPTH_STENCIL_VIEW_DESC desc = {};
		desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
		desc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
		desc.Flags = 0;
		desc.Texture2D.MipSlice = 0;

		HRESULT hr = m_device->CreateDepthStencilView(resource, &desc, &view->m_DSV);
		if (!SUCCEEDED(hr)) {
			ERROR_AND_DIE("Can't create SRV of texture resource");
		}
		std::string debugName = Stringf("Texture - DSV : fomat:%d", config.m_format);
		SetObjDebugName(view->m_DSV, debugName.c_str());
		break;
	}
	default:
		break;
	}
	return view;
}

RendererConfig const& Renderer::GetConfig() const
{
	// TODO: insert return statement here
	return m_config;
}

void Renderer::SetObjDebugName(ID3D11DeviceChild* object, char const* name)
{
#if defined(ENGINE_DEBUG_RENDER)
	object->SetPrivateData(WKPDID_D3DDebugObjectName, (UINT)strlen(name), name);
#else
	UNUSED(object);
	UNUSED(name);
#endif
}

bool Renderer::CreateInputLayoutFromVS(std::vector<uint8_t>& shaderByteCode, ID3D11InputLayout** pInputLayout)
{
	// Reflect shader info
	ID3D11ShaderReflection* pVertexShaderReflection = NULL;
	if (FAILED(D3DReflect((void*)shaderByteCode.data(), shaderByteCode.size(), IID_ID3D11ShaderReflection, (void**)&pVertexShaderReflection)))
	{
		return false;
	}

	// Get shader info
	D3D11_SHADER_DESC shaderDesc;
	pVertexShaderReflection->GetDesc(&shaderDesc);

	// Read input layout description from shader info
	std::vector<D3D11_INPUT_ELEMENT_DESC> inputLayoutDesc;
	for (UINT i = 0; i < shaderDesc.InputParameters; i++)
	{
		D3D11_SIGNATURE_PARAMETER_DESC paramDesc;
		pVertexShaderReflection->GetInputParameterDesc(i, &paramDesc);

		// fill out input element desc
		D3D11_INPUT_ELEMENT_DESC elementDesc;
		elementDesc.SemanticName = paramDesc.SemanticName;
		elementDesc.SemanticIndex = paramDesc.SemanticIndex;
		elementDesc.InputSlot = 0;
		elementDesc.AlignedByteOffset = (i == 0) ? 0 : D3D11_APPEND_ALIGNED_ELEMENT;
		elementDesc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		elementDesc.InstanceDataStepRate = 0;

		// determine DXGI format
		if (paramDesc.Mask == 1)
		{
			if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) elementDesc.Format = DXGI_FORMAT_R32_UINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) elementDesc.Format = DXGI_FORMAT_R32_SINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) elementDesc.Format = DXGI_FORMAT_R32_FLOAT;
		}
		else if (paramDesc.Mask <= 3)
		{
			if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) elementDesc.Format = DXGI_FORMAT_R32G32_UINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) elementDesc.Format = DXGI_FORMAT_R32G32_SINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) elementDesc.Format = DXGI_FORMAT_R32G32_FLOAT;
		}
		else if (paramDesc.Mask <= 7)
		{
			if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) elementDesc.Format = DXGI_FORMAT_R32G32B32_UINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) elementDesc.Format = DXGI_FORMAT_R32G32B32_SINT;
			else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) elementDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT;
		}
		else if (paramDesc.Mask <= 15)
		{
			if (AreStringsEqualCaseInsensitive(elementDesc.SemanticName, "COLOR") || AreStringsEqualCaseInsensitive(elementDesc.SemanticName, "TINT")) {
				elementDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			}
			else {
				if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_UINT32) elementDesc.Format = DXGI_FORMAT_R32G32B32A32_UINT;
				else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_SINT32) elementDesc.Format = DXGI_FORMAT_R32G32B32A32_SINT;
				else if (paramDesc.ComponentType == D3D_REGISTER_COMPONENT_FLOAT32) elementDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
			}


		}

		//save element desc
		inputLayoutDesc.push_back(elementDesc);
	}

	// Try to create Input Layout
	HRESULT hr = m_device->CreateInputLayout(&inputLayoutDesc[0], (UINT)inputLayoutDesc.size(), (void*)shaderByteCode.data(), (UINT)shaderByteCode.size(), pInputLayout);
	if (!SUCCEEDED(hr)) {
		ERROR_AND_DIE("SOMETHING WENT WRONG CREATING INPUT LAYOUT");
	}

	//Free allocation shader reflection memory
	pVertexShaderReflection->Release();

	return true;
}