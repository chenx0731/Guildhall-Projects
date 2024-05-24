#pragma once
#include <vector>
#include "Engine/Core/Vertex_PNCU.hpp"


class CPUMesh
{
public:
	CPUMesh(std::vector<Vertex_PCUTBN> const& vertexes, std::vector<unsigned int> const& indexes);
	~CPUMesh();
	void CalculateTBNVectors();
public:
	std::vector<Vertex_PCUTBN>	m_vertexes;
	std::vector<unsigned int>	m_indexes;
};