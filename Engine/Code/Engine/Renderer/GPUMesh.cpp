#include "Engine/Renderer/GPUMesh.hpp"
#include "Engine/Renderer/CPUMesh.hpp"
#include "Engine/Renderer/VertexBuffer.hpp"
#include "Engine/Renderer/IndexBuffer.hpp"

GPUMesh::GPUMesh(VertexBuffer*& vertexBuffer, IndexBuffer*& indexBuffer) : m_vertexBuffer(vertexBuffer) , m_indexBuffer(indexBuffer)
{
	
}

GPUMesh::~GPUMesh()
{
	delete m_vertexBuffer;
	delete m_indexBuffer;
}

