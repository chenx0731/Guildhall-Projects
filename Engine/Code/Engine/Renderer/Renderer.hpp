#pragma once
#include "Game/EngineBuildPreferences.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Math/Mat44.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Core/StringUtils.hpp"
#include <stdint.h>
#include <filesystem>
#include <vector>
#define DX_SAFE_RELEASE(dxObject)	\
{									\
	if ((dxObject) != nullptr)		\
	{								\
		(dxObject)->Release();		\
		(dxObject) = nullptr;		\
	}								\
}
//#include "Engine/Renderer/Texture.hpp"
//#include "Engine/Renderer/Window.hpp"
enum class Topology {
	LineList,
	TriangleList,
	PointList
};

class Window;
class Texture;
class StructuredBuffer;
struct BufferConfig;
struct TextureConfig;
struct ShaderConfig;
class Shader;
class ResourceView;
struct ResourceViewConfig;
struct IntVec2;
class BitmapFont;
class VertexBuffer;
class IndexBuffer;
class GPUMesh;
class CPUMesh;
class ConstantBuffer;
class UnorderedAccessView;
class Image;
struct Vertex_PCUTBN;

struct 	ID3D11Device;
struct	ID3D11DeviceContext;	
struct  ID3D11InputLayout;
struct	IDXGISwapChain;			
struct	ID3D11RenderTargetView;
struct  ID3D11ShaderResourceView;
struct	ID3D11RasterizerState;	
struct  ID3D11BlendState;
struct  ID3D11SamplerState;
struct	ID3D11DepthStencilState;
struct	ID3D11DepthStencilView;
struct  ID3D11DeviceChild;
struct	ID3D11Texture2D;
struct  ID3D11Buffer;
struct  ID3D11Resource;
struct  ID3D11UnorderedAccessView;

struct RendererConfig
{
	Window* m_window = nullptr;
};

enum class BlendMode
{
	ALPHA,
	ADDITIVE,
	OPAQUE,
	COUNT
};

enum class SamplerMode
{
	POINT_CLAMP,
	BILINEAR_WRAP,
	COUNT
};

enum class RasterizerMode
{
	SOLID_CULL_NONE,
	SOLID_CULL_BACK,
	WIREFRAME_CULL_NONE,
	WIREFRAME_CULL_BACK,
	COUNT
};

enum class DepthMode
{
	DISABLED,
	ENABLED,
	COUNT
};

class Renderer
{
public:
	Renderer(RendererConfig const& config);
	~Renderer();

	void Startup();
	void BeginFrame();
	void EndFrame();
	void Shutdown();

	//void ClearScreen( const Rgba8& clearColor );
	void ClearScreen( const Rgba8& clearColor, Texture const* depthStencil = nullptr, Texture const* renderTarget = nullptr );
	void ClearRenderTarget(const Rgba8& clearColor, Texture const* renderTarget = nullptr);
	void ClearDepthStencil(const Rgba8& clearColor, Texture const* depthStencil = nullptr);
	//void ClearState
	void BeginCamera(const Camera& camera);
	void EndCamera(const Camera& camera);

	void DispatchCS(int threadX, int threadY, int threadZ);
	void DrawVertexArray(int numVertexes, const Vertex_PCU* vertexes, Topology topology = Topology::TriangleList);
	void DrawVertexArray(int numVertexes, const Vertex_PCUTBN* vertexes);
	void DrawVertexIndexArray(int numVertexes, int numIndexes, const Vertex_PCU* vertexes, const unsigned int* indexes, VertexBuffer* vertexBuffer = nullptr, IndexBuffer* indexBuffer = nullptr);
	void DrawVertexIndexArray(int numVertexes, int numIndexes, const Vertex_PCUTBN* vertexes, const unsigned int* indexes, VertexBuffer* vertexBuffer = nullptr, IndexBuffer* indexBuffer = nullptr);
	void DrawVertexBuffer(VertexBuffer* vbo, int vertexCount, Topology topology = Topology::TriangleList, int vertexOffset = 0);
	void DrawVertexIndexBuffer(VertexBuffer* vbo, IndexBuffer* ibo, size_t indexCount);
	void Draw(int vertexCount, int vertexOffset = 0);
	void DrawIndexed(unsigned int IndexCount, unsigned int StartIndexLocation, int BaseVertexLocation);

	Texture*	CreateOrGetTextureFromFile(char const* imageFilePath);
	Texture*	GetTextureForFileName(char const* imageFilePath);
	Texture*	CreateTextureFromData(char const* name, IntVec2 dimensions, int bytesPerTexel, uint8_t* texelData);
	Texture*	CreateTextureFromImage(const Image& image);
	Texture*	CreateCubemapTextureFromFiles(Strings files);
	Texture*			CreateTextureByConfig(TextureConfig config);
	StructuredBuffer*	CreateStructuredBufferByData(size_t numOfElements, size_t stride, void* data);
	StructuredBuffer*	CreateStructuredBufferByConfig(BufferConfig config);
	void		BindTexture(Texture* texture, int slot = 0);
	void		BindBackBufferAsTexture(bool isBinding = true, int slot = 0);

	void		BindTextures(Texture* texture0, Texture* texture1, Texture* texture2);
	void		BindShader(Shader* shader);

	void		IASetVertexBuffers(VertexBuffer* vbo);
	void		IASetPrimitiveTopologyToPointList();
	void		IASetPrimitiveTopology(Topology topology);
	void		IASetInputLayout(Shader* shader);
	void		VSSetShader(Shader* shader);
	void		GSSetShader(Shader* shader);
	void		PSSetShader(Shader* shader);
	void		CSSetShader(Shader* shader);
	void		SetDepthTexture(Texture const* texture, Texture const* renderTarget = nullptr);

	void		BindDepthStencil();
	void		UnbindDepthStencil(Texture const* renderTarget = nullptr);

	Shader*		CreateShader(char const* shaderName, char const* shaderSource);
	Shader*		CreateShader(char const* shaderName);
	Shader*		CreateShaderByConfig(ShaderConfig config);
	Shader*		CreateComputeShader(std::filesystem::path shaderName);
	//Shader*		CreateComputeShaderByConfig(std::filesystem::path shaderName);
	//void		CreateGeometryShader()
	bool		CompileShaderToByteCode(std::vector<unsigned char>& outByteCode, char const* name, char const* source, char const* entryPoint, char const* target);

	void		CreateBlendStates();
	void		CreateSamplerStates();
	void		CreateRasterizerStates();
	void		CreateDepthStates();
	
	void		SetStatesIfChanged();
	void		SetBlendMode(BlendMode blendMode);
	void		SetSamplerMode(SamplerMode samplerMode);
	void		SetRasterizerMode(RasterizerMode rasterizerMode);
	void		SetDepthMode(DepthMode depthMode);

	void		CSSetUAV(StructuredBuffer* buffer, int slot) const;
	void		CSSetUAV(Texture* texture, int slot) const;

	void		CSSetSRV(StructuredBuffer* buffer, int slot) const;
	void		CSSetSRV(Texture* texture, int slot) const;
	void		VSSetSRV(StructuredBuffer* buffer, int slot) const;
	void		VSSetSRV(Texture* buffer, int slot) const;


	void		SetRenderTarget() const;
	void		SetTextureAsRenderTarget(const Texture* texture) const;

	VertexBuffer* CreateVertexBuffer(const size_t size, const unsigned int stride);
	IndexBuffer* CreateIndexBuffer(const size_t size);
	ID3D11Buffer* CreateOutputBuffer(const size_t size, const unsigned int stride);
	ID3D11Buffer* CreateInputBuffer(const size_t size, const unsigned int stride);
	GPUMesh*	 CreateGPUMeshFromCPUMesh(const CPUMesh* cpuMesh);
	void CopyCPUToGPU(const void* data, size_t size, unsigned int stride, VertexBuffer*& vbo);
	void CopyCPUToGPU(const void* data, size_t size, IndexBuffer*& ibo);
	void CopyResources(void* dst, const void* src);
	void CopyCPUToGPU(void* data, size_t size, UnorderedAccessView* uav);
	void CopyGPUToCPU(void* data, size_t size, ID3D11Buffer* outBuffer);
	void CopyCPUToGPU(void* data, size_t size, ID3D11Buffer* inBuffer);
	void BindVertexBuffer(VertexBuffer* vbo, Topology Topology = Topology::TriangleList);
	void BindIndexBuffer(IndexBuffer* ibo);

	void		UpdateConstantBuffer(ConstantBuffer* cbo, const void* gpuData, size_t size);
	void		SetModelConstants(const Mat44& modelMatrix = Mat44(), const Rgba8& modelColor = Rgba8::WHITE);
	void		SetLightConstants(const Vec3& sunDirection = Vec3(),
		const float sunIntensity = 0.f, const Rgba8& ambientColor = Rgba8(),
		Vec3 worldEyePos = Vec3(), int normalMode = 0, int specularMode = 0, float specularIntensity = 1.f, float specularPower = 32.f);
	ConstantBuffer* CreateConstantBuffer(const size_t size);
	void CopyCPUToGPU(const void* data, size_t size, ConstantBuffer*& cbo);
	void BindConstantBuffer(int slot, ConstantBuffer* cbo);
	void VSBindConstantBuffer(int slot, ConstantBuffer* cbo);
	void PSBindConstantBuffer(int slot, ConstantBuffer* cbo);
	void GSBindConstantBuffer(int slot, ConstantBuffer* cbo);
	void CSBindConstantBuffer(int slot, ConstantBuffer* cbo);

	BitmapFont* CreateOrGetBitmapFont(const char* bitmapFontFilePathWithNoExtension);
	BitmapFont* GetBitmapFontForFileName(const char* bitmapFontFilePathWithNoExtension);

	ID3D11Device*			GetD3D11Device() const	{ return m_device;}
	ID3D11DeviceContext*	GetD3D11DeviceContent() const { return m_deviceContext;}
	IDXGISwapChain*			GetSwapChain() const { return m_swapChain;}
	ID3D11RenderTargetView* GetD3D11TargetView() const { return m_targetView; }

	ResourceView*			GetOrCreateView(ResourceViewConfig config, ID3D11Buffer* resource);
	ResourceView*			GetOrCreateView(ResourceViewConfig config, ID3D11Resource* resource);

	RendererConfig const& GetConfig() const;
	Texture* m_depthStencilTexture = nullptr;
	//Texture* m_depthStencilGameTexture = nullptr;
public:
	ID3D11Device* m_device = nullptr;
	ID3D11DeviceContext* m_deviceContext = nullptr;

private:
	bool		CreateInputLayoutFromVS(std::vector<uint8_t>& shaderByteCode, ID3D11InputLayout** pInputLayout);
	BitmapFont* CreateBitmapFont(const char* bitmapFontFilePathWithNoExtension);
	Texture*	CreateTextureFromFile(char const* imageFilePath);
	void		SetObjDebugName(ID3D11DeviceChild* object, char const* name);

protected:
	RendererConfig				m_config;
	std::vector<Texture*>		m_loadedTextures;
	std::vector<BitmapFont*>	m_loadedFonts;
	std::vector<Shader*>		m_loadedShaders;
	Shader*						m_currentShader = nullptr;
	Shader*						m_defaultShader = nullptr;
	Shader*						m_copyShader = nullptr;
	Texture*					m_defaultTexture = nullptr;
	

	VertexBuffer*				m_immediateVBO_PCU	= nullptr;
	VertexBuffer*				m_immediateVBO_PNCU	= nullptr;

	IndexBuffer*				m_immediateIBO = nullptr;

	ConstantBuffer*				m_lightCBO		= nullptr;
	ConstantBuffer*				m_cameraCBO		= nullptr;
	ConstantBuffer*				m_modelCBO		= nullptr;
	
	IDXGISwapChain*				m_swapChain				= nullptr;
	ID3D11RenderTargetView*		m_targetView			= nullptr;
	Texture*					m_preBackBuffer			= nullptr;
	//ID3D11DepthStencilView*		m_depthStencilView		= nullptr;
	//ID3D11Texture2D*			m_depthStencilTexture	= nullptr;

	ID3D11RasterizerState*		m_rasterizerState = nullptr;
	ID3D11BlendState*			m_blendState = nullptr;
	ID3D11SamplerState*			m_samplerState = nullptr;
	ID3D11DepthStencilState*	m_depthStencilState = nullptr;

	BlendMode					m_desiredBlendMode = BlendMode::ALPHA;
	SamplerMode					m_desiredSamplerMode = SamplerMode::POINT_CLAMP;
	RasterizerMode				m_desiredRasterizerMode = RasterizerMode::SOLID_CULL_BACK;
	DepthMode					m_desiredDepthMode = DepthMode::ENABLED;

	ID3D11BlendState*			m_blendStates[(int)(BlendMode::COUNT)] = {};
	ID3D11SamplerState*			m_samplerStates[(int)(SamplerMode::COUNT)] = {};
	ID3D11RasterizerState*		m_rasterizerStates[(int)(RasterizerMode::COUNT)] = {};
	ID3D11DepthStencilState*	m_depthStencilStates[(int)(DepthMode::COUNT)] = {};

	void*					m_dxgiDebug = nullptr;
	void*					m_dxgiDebugModule = nullptr;
};