#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/CPUMesh.hpp"
#include "Engine/Renderer/VertexBuffer.hpp"
#include "Engine/Core/VertexUtils.hpp"

CPUMesh::CPUMesh(std::vector<Vertex_PCUTBN> const& vertexes, std::vector<unsigned int> const& indexes)
{
	m_vertexes = vertexes;
	m_indexes = indexes;
}

CPUMesh::~CPUMesh()
{
}

void CPUMesh::CalculateTBNVectors()
{
	CalculateTangentSpaceBasisVectors(m_vertexes, m_indexes);
}
