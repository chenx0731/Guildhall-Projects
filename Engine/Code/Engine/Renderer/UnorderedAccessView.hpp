#pragma once
#include "Engine/Renderer/Texture.hpp"

struct ID3D11UnorderedAccessView;
struct ID3D11ShaderResourceView;
struct ID3D11Buffer;
struct ID3D11Device;

enum class UAVType {
	RAW_TEXTURE2D,
	STRUCTURED,
};

class UnorderedAccessView
{
	friend class Renderer;

public:
	UnorderedAccessView(ID3D11Device* device, void* initialData, size_t numElements, size_t stride, UAVType type = UAVType::RAW_TEXTURE2D);
	~UnorderedAccessView();

private:
	void CreateStructuredUAV(void* data);

// just for test 
public:
	ID3D11UnorderedAccessView*	m_UAV = nullptr;
	ID3D11ShaderResourceView*	m_SRV = nullptr;
	ID3D11Buffer* m_buffer = nullptr;
	ID3D11Device* m_device = nullptr;

	size_t m_numElements = 0;
	size_t m_stride = 0;
	UAVType m_type = UAVType::RAW_TEXTURE2D;
};