#pragma once
#include <string>
#define DX_SAFE_RELEASE(dxObject)	\
{									\
	if ((dxObject) != nullptr)		\
	{								\
		(dxObject)->Release();		\
		(dxObject) = nullptr;		\
	}								\
}
struct ID3D11Buffer;


class VertexBuffer
{
	friend class Renderer;
public:
	VertexBuffer(size_t size, unsigned int stride);
	VertexBuffer(const VertexBuffer& copy) = delete;
	virtual ~VertexBuffer();

	unsigned int GetStride() const;
public:
	ID3D11Buffer* m_buffer = nullptr;
	size_t m_size = 0;
	unsigned int m_stride = 0;

	void* m_dxgiDebug = nullptr;
	void* m_dxgiDebugModule = nullptr;
};