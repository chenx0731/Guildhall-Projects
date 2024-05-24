#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/UnorderedAccessView.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
//#define WIN32_LEAN_AND_MEAN
//#pragma comment (lib, "d3d11.lib")
//#pragma comment (lib, "dxgi.lib")
//#pragma comment (lib, "d3dcompiler.lib")
//#if defined(ENGINE_DEBUG_RENDER)
//#include <dxgidebug.h>
//#pragma comment(lib, "dxguid.lib")
//#endif
//#include <windows.h>
#include <d3d11.h>
//#include <dxgi.h>
//#include <d3dcompiler.h>



UnorderedAccessView::UnorderedAccessView(ID3D11Device* device, void* initialData, size_t numElements, size_t stride, UAVType type)
	: m_device(device), m_numElements(numElements), m_stride(stride), m_type(type)
{
	switch (type)
	{
	case UAVType::RAW_TEXTURE2D:
		break;
	case UAVType::STRUCTURED:
		CreateStructuredUAV(initialData);
		break;
	default:
		ERROR_AND_DIE("UAV: Unknown type");
		break;
	}
}

UnorderedAccessView::~UnorderedAccessView()
{
	DX_SAFE_RELEASE(m_UAV);
	DX_SAFE_RELEASE(m_SRV);
	DX_SAFE_RELEASE(m_buffer);
}

void UnorderedAccessView::CreateStructuredUAV(void* data)
{
	D3D11_BUFFER_DESC descBuffer = {};
	descBuffer.BindFlags = D3D11_BIND_UNORDERED_ACCESS | D3D11_BIND_SHADER_RESOURCE;
	descBuffer.ByteWidth = UINT(m_numElements * m_stride);
	descBuffer.MiscFlags = D3D11_RESOURCE_MISC_BUFFER_STRUCTURED;

	descBuffer.StructureByteStride = (UINT)m_stride;
	// remove this hack later
	//descBuffer.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	//descBuffer.Usage = D3D11_USAGE_DYNAMIC;

	D3D11_SUBRESOURCE_DATA* initialData = nullptr;

	if (data) {
		initialData = new D3D11_SUBRESOURCE_DATA();
		initialData->pSysMem = data;
		initialData->SysMemPitch = UINT(m_numElements * m_stride);
	}

	HRESULT bufferCreation = m_device->CreateBuffer(&descBuffer, initialData, &m_buffer);
	if (!SUCCEEDED(bufferCreation)) {
		ERROR_AND_DIE("UAV: Couldn't create buffer");
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc = {};

	srvDesc.ViewDimension = D3D11_SRV_DIMENSION_BUFFEREX;
	srvDesc.BufferEx.FirstElement = 0;
	srvDesc.BufferEx.NumElements = (UINT)m_numElements;
	srvDesc.Format = DXGI_FORMAT_UNKNOWN;

	HRESULT srvCreation = m_device->CreateShaderResourceView(m_buffer, &srvDesc, &m_SRV);
	if (!SUCCEEDED(srvCreation)) {
		ERROR_AND_DIE("UAV: Couldn't create shader resource view");
	}

	D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc = {};
	uavDesc.ViewDimension = D3D11_UAV_DIMENSION_BUFFER;
	uavDesc.Format = DXGI_FORMAT_UNKNOWN;
	uavDesc.Buffer.FirstElement = 0;
	uavDesc.Buffer.NumElements = (UINT)m_numElements;
	//uavDesc.Buffer.Flags = D3D11_BUFFER_UAV_FLAG_APPEND;

	HRESULT uavCreation = m_device->CreateUnorderedAccessView(m_buffer, &uavDesc, &m_UAV);
	if (!SUCCEEDED(uavCreation)) {
		ERROR_AND_DIE("UAV: Couldn't create UAV");
	}
}
