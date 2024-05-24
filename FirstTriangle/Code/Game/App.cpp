#define UNUSED(x) (void)(x);
#define WIN32_LEAN_AND_MEAN
#if defined(_DEBUG)
#define ENGINE_DEBUG_RENDER
#endif
#pragma comment (lib, "d3d11.lib")
#pragma comment (lib, "dxgi.lib")
#pragma comment (lib, "d3dcompiler.lib")
#if defined(ENGINE_DEBUG_RENDER)
	#include <dxgidebug.h>
	#pragma comment(lib, "dxguid.lib")
#endif
#define DX_SAFE_RELEASE(dxObject)	\
{									\
	if ((dxObject) != nullptr)		\
	{								\
		(dxObject)->Release();		\
		(dxObject) = nullptr;		\
	}								\
}
#include <windows.h>
#include <d3d11.h>
#include <dxgi.h>
#include <d3dcompiler.h>
#include <vector>
#include <string>
#include "Engine/Core/Time.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Game/App.hpp"
#include "Game/GameCommon.hpp"


//Game* g_theGame = nullptr;
InputSystem* g_theInput = nullptr;
RandomNumberGenerator* g_rng = nullptr;
AudioSystem* g_theAudio = nullptr;
Window* g_theWindow = nullptr;

ID3D11Device*			m_device			= nullptr;
ID3D11DeviceContext*	m_deviceContext		= nullptr;
IDXGISwapChain*			m_swapChain			= nullptr;
ID3D11RenderTargetView* m_targetView		= nullptr;

ID3D11VertexShader*		m_vertexShader		= nullptr;
ID3D11PixelShader*		m_pixelShader		= nullptr;
ID3D11InputLayout*		m_inputLayout		= nullptr;
ID3D11Buffer*			m_buffer			= nullptr;
ID3D11RasterizerState*	m_rasterizerState	= nullptr;

void*					m_dxgiDebugModule	= nullptr;
void*					m_dxgiDebug			= nullptr;

std::vector<uint8_t>	m_vertexShaderByteCode;
std::vector<uint8_t>	m_pixelShaderByteCode;

const char* shaderSource = R"(
struct vs_input_t
{
	float3 localPosition : POSITION;
	float4 color : COLOR;
	float2 uv : TEXCOORD;
};

struct v2p_t
{
	float4 position : SV_Position;
	float4 color : COLOR;
	float2 uv : TEXCOORD;
};

v2p_t VertexMain(vs_input_t input)
{
	v2p_t v2p;
	v2p.position = float4(input.localPosition, 1);
	v2p.color = input.color;
	v2p.uv = input.uv;
	return v2p;
}

float4 PixelMain(v2p_t input) : SV_Target0
{
	return float4(input.color);
}
)";

Vertex_PCU vertices[] = {
	Vertex_PCU(Vec2(-0.5f, -0.5f), Rgba8(255, 255, 255, 255), Vec2(0.f, 0.f)),
	Vertex_PCU(Vec2(0.0f, 0.5f), Rgba8(255, 255, 255, 255), Vec2(0.f, 0.f)),
	Vertex_PCU(Vec2(0.5f, -0.5f), Rgba8(255, 255, 255, 255), Vec2(0.f, 0.f)),
};

D3D11_INPUT_ELEMENT_DESC inputElementDesc[] =
{
	{"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"COLOR", 0, DXGI_FORMAT_R8G8B8A8_UNORM, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
	{"TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0},
};

App::App()
{
	
}

App::~App()
{

}

void App::Startup()
{	
	InputSystemConfig inputSystemConfig;
	g_theInput = new InputSystem(inputSystemConfig);
	
	WindowConfig windowConfig;
	windowConfig.m_windowTitle = "First Triangle";
	windowConfig.m_clientAspect = 2.0f;
	windowConfig.m_inputSystem = g_theInput;
	g_theWindow = new Window(windowConfig);


	AudioSystemConfig audioSystemConfig;
	g_theAudio = new AudioSystem(audioSystemConfig);
	
	g_rng = new RandomNumberGenerator();

	g_theInput->Startup();
	g_theWindow->Startup();
	g_theAudio->Startup();

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
							
	DXGI_SWAP_CHAIN_DESC	m_swapChainDesc = {0};
	m_swapChainDesc.BufferDesc.Width	= g_theWindow->GetClientDimensions().x;
	m_swapChainDesc.BufferDesc.Height	= g_theWindow->GetClientDimensions().y;
	m_swapChainDesc.BufferDesc.Format	= DXGI_FORMAT_R8G8B8A8_UNORM;
	m_swapChainDesc.SampleDesc.Count	= 1;
	m_swapChainDesc.BufferUsage			= DXGI_USAGE_RENDER_TARGET_OUTPUT;
	m_swapChainDesc.BufferCount			= 2;
	m_swapChainDesc.OutputWindow		= (HWND)(g_theWindow->GetHwnd());
	m_swapChainDesc.Windowed			= true;
	m_swapChainDesc.SwapEffect			= DXGI_SWAP_EFFECT_FLIP_DISCARD;
	
	HRESULT hr;
	hr = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL,
		D3D11_CREATE_DEVICE_DEBUG, NULL, NULL, D3D11_SDK_VERSION,
		&m_swapChainDesc, &m_swapChain, &m_device, NULL, &m_deviceContext);

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
	if (!SUCCEEDED(hr)) {
		ERROR_AND_DIE("Could create render target view for swap chain buffer.");
	}

	//backBuffer->Release();
	DX_SAFE_RELEASE(backBuffer);

	// compile the vertex shader
	UINT flags = 0;
#if defined( ENGINE_DEBUG_RENDER )
	flags |= D3DCOMPILE_DEBUG;
#endif

	std::string s_shaderSource = shaderSource;

	ID3DBlob* vertexShaderBlob = nullptr;
	ID3DBlob* vertexErrorBlob = nullptr;
	hr = D3DCompile(shaderSource, s_shaderSource.length(), NULL, NULL, NULL, "VertexMain", "vs_5_0", flags, NULL, &vertexShaderBlob, &vertexErrorBlob);
	if (!SUCCEEDED(hr)) {
		ERROR_AND_DIE("Could not compile the vertex shader.");
	}

	// create the vertex shader
	//ID3D11VertexShader* vertexShader = nullptr;
	//SIZE_T bufferSize = vertexShaderBlob->GetBufferSize();
	hr = m_device->CreateVertexShader(vertexShaderBlob->GetBufferPointer(), vertexShaderBlob->GetBufferSize(), NULL, &m_vertexShader);
	if (!SUCCEEDED(hr)) {
		ERROR_AND_DIE("Could not create the vertex shader.");
	}

	// compile the pixel shader
	ID3DBlob* pixelShaderBlob = nullptr;
	ID3DBlob* pixelErrorBlob = nullptr;

	DWORD compileFlags = D3DCOMPILE_OPTIMIZATION_LEVEL3;
#if defined(ENGINE_DEBUG_RENDER)
	compileFlags = D3DCOMPILE_DEBUG;
	compileFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	hr = D3DCompile(shaderSource, s_shaderSource.length(), NULL, NULL, NULL, "PixelMain", "ps_5_0", compileFlags, NULL, &pixelShaderBlob, &pixelErrorBlob);
	if (!SUCCEEDED(hr)) {
		ERROR_AND_DIE("Could not compile the pixel shader.");
	}

	// create the pixel shader
	//ID3D11PixelShader* pixelShader = nullptr;
	hr = m_device->CreatePixelShader(pixelShaderBlob->GetBufferPointer(), pixelShaderBlob->GetBufferSize(), NULL, &m_pixelShader);
	if (!SUCCEEDED(hr)) {
		ERROR_AND_DIE("Could not create the pixel shader.");
	}

	// create input layout
	//ID3D11InputLayout* inputLayout = nullptr;
	hr = m_device->CreateInputLayout(inputElementDesc, 
		sizeof(inputElementDesc) / sizeof(inputElementDesc[0]), 
		vertexShaderBlob->GetBufferPointer(), 
		(SIZE_T)vertexShaderBlob->GetBufferSize(), 
		&m_inputLayout);
	if (!SUCCEEDED(hr)) {
		ERROR_AND_DIE("Could not create the input layout.");
	}

	// create vertex buffer
	D3D11_BUFFER_DESC bufferDesc = { 0 };
	bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	bufferDesc.ByteWidth = (UINT)vertexShaderBlob->GetBufferSize();
	bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	DX_SAFE_RELEASE(vertexShaderBlob);
	DX_SAFE_RELEASE(vertexErrorBlob);
	DX_SAFE_RELEASE(pixelShaderBlob);
	DX_SAFE_RELEASE(pixelErrorBlob);


	//ID3D11Buffer* vertexBuffer = nullptr;
	hr = m_device->CreateBuffer(&bufferDesc, NULL, &m_buffer);
	if (!SUCCEEDED(hr)) {
		ERROR_AND_DIE("Could not create vertex buffer.");
	}

	D3D11_MAPPED_SUBRESOURCE mappedSubresource;

	hr = m_deviceContext->Map(m_buffer, NULL, D3D11_MAP_WRITE_DISCARD, NULL, &mappedSubresource);
	if (!SUCCEEDED(hr)) {
		ERROR_AND_DIE("Could not map content.");
	}

	memcpy(mappedSubresource.pData, &vertices, sizeof(vertices));
	m_deviceContext->Unmap(m_buffer, NULL);

	// set viewport begin camera
	D3D11_VIEWPORT viewPort = { 0 };
	viewPort.TopLeftX = 0;
	viewPort.TopLeftY = 0;
	viewPort.Width = (float)g_theWindow->GetClientDimensions().x;
	viewPort.Height = (float)g_theWindow->GetClientDimensions().y;
	viewPort.MinDepth = 0;
	viewPort.MaxDepth = 1;

	m_deviceContext->RSSetViewports(1, &viewPort);

	// set the rasterizer state start up
	D3D11_RASTERIZER_DESC rasterizerDesc = {};
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.CullMode = D3D11_CULL_NONE;
	rasterizerDesc.DepthClipEnable = true;
	rasterizerDesc.AntialiasedLineEnable = true;

	//ID3D11RasterizerState* rasterizerState = nullptr;
	hr = m_device->CreateRasterizerState(&rasterizerDesc, &m_rasterizerState);
	if (!SUCCEEDED(hr)) {
		ERROR_AND_DIE("Could not create rasterizer state.");
	}

	m_deviceContext->RSSetState(m_rasterizerState);

	
	// set pipeline state
	UINT strides = sizeof(Vertex_PCU);
	UINT offsets = 0;

	m_deviceContext->IASetVertexBuffers(NULL, 1, &m_buffer, &strides, &offsets);

	m_deviceContext->IASetInputLayout(m_inputLayout);

	m_deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	m_deviceContext->VSSetShader(m_vertexShader, NULL, NULL);

	m_deviceContext->PSSetShader(m_pixelShader, NULL, NULL);
}

void App::Shutdown()
{

	g_theInput->Shutdown();
	g_theWindow->Shutdown();
	g_theAudio->Shutdown();


	delete g_theInput;
	delete g_theWindow;
	delete g_theAudio;

	delete g_rng;


	g_theInput = nullptr;
	g_theWindow = nullptr;
	g_theAudio = nullptr;

	g_rng = nullptr;

	DX_SAFE_RELEASE(m_device);
	DX_SAFE_RELEASE(m_deviceContext);
	DX_SAFE_RELEASE(m_swapChain);
	DX_SAFE_RELEASE(m_targetView);
	DX_SAFE_RELEASE(m_vertexShader);
	DX_SAFE_RELEASE(m_pixelShader);
	DX_SAFE_RELEASE(m_inputLayout);
	DX_SAFE_RELEASE(m_buffer);
	DX_SAFE_RELEASE(m_rasterizerState);


#if defined(ENGINE_DEBUG_RENDER)
	((IDXGIDebug*)m_dxgiDebug)->ReportLiveObjects(DXGI_DEBUG_ALL, (DXGI_DEBUG_RLO_FLAGS)(DXGI_DEBUG_RLO_DETAIL | DXGI_DEBUG_RLO_IGNORE_INTERNAL));
	((IDXGIDebug*)m_dxgiDebug)->Release();
	m_dxgiDebug = nullptr;
	::FreeLibrary((HMODULE)m_dxgiDebugModule);
	m_dxgiDebugModule = nullptr;
#endif

}

void App::RunFrame()
{
	static float lastTime = static_cast<float> (GetCurrentTimeSeconds());
	float currentTime = static_cast<float> (GetCurrentTimeSeconds());
	float _deltaSecond = currentTime - lastTime;
	if (_deltaSecond > 0.1f)
	{
		_deltaSecond = 0.1f;
	}
	if (m_isSlowMo)
	{
		_deltaSecond *= 0.1f;
	}
	lastTime = currentTime;
	BeginFrame();
	Update(_deltaSecond);
	Render();
	EndFrame();
}

void App::Run()
{
	while (!m_isQuitting)
	{
		RunFrame();
	}
}

bool App::HandleQuitRequested()
{
	m_isQuitting = true;
	return false;
}

void App::BeginFrame()
{
	g_theInput->BeginFrame();
	g_theWindow->BeginFrame();
	g_theAudio->BeginFrame();
}

void App::Update(float deltaSeconds)
{	
	UpdateFromKeyboard(deltaSeconds);
}

void App::Render() const
{
	// begin frame
	m_deviceContext->OMSetRenderTargets(1, &m_targetView, NULL);

	Rgba8 m_color = Rgba8::GREY;
	float m_colorRGBA[4];
	m_color.GetAsFloats(m_colorRGBA);
	m_deviceContext->ClearRenderTargetView(m_targetView, m_colorRGBA);
	// draw
	m_deviceContext->Draw(3, NULL);

	// end frame
	HRESULT hr;
	hr = m_swapChain->Present(0, 0);
	if (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET)
	{
		ERROR_AND_DIE("Device has been lost, application will be terminated.");
	}
}

void App::EndFrame()
{
	g_theInput->EndFrame();
	g_theAudio->EndFrame();
}

void SetDebugName(ID3D11DeviceChild* object, char const* name)
{
#if defined(ENGINE_DEBUG_RENDER)
	HRESULT hr = object->SetPrivateData(WKPDID_D3DDebugObjectName, (UINT)strlen(name), name);
	if (!SUCCEEDED(hr)) {
		ERROR_AND_DIE(Stringf("Could not set debug name \"%s\".", name));
	}
#else
	UNUSED(object);
	UNUSED(name);
#endif
}


void App::UpdateFromKeyboard(float deltaSeconds)
{
	UNUSED(deltaSeconds);
	if (g_theInput->WasKeyJustPressed(KEYCODE_ESC) || g_theInput->GetController(0).WasButtonJustPressed(XBOX_BUTTON_BACK))
	{
		 HandleQuitRequested();
	}
}
