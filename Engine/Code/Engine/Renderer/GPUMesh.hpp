#pragma once
class VertexBuffer;
class IndexBuffer;
class CPUMesh;
class Renderer;

class GPUMesh
{
	friend class Renderer;
public:
	GPUMesh(VertexBuffer*& vertexBuffer, IndexBuffer*& indexBuffer);
	GPUMesh(const GPUMesh& copy) = delete;
	~GPUMesh();

public:
	VertexBuffer* m_vertexBuffer = nullptr;
	IndexBuffer* m_indexBuffer = nullptr;
};