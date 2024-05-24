#pragma once
#include "Game/Entity.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Core/Rgba8.hpp"
#include <vector>

class CPUMesh;
class GPUMesh;
class Material;
class Texture;
class VertexBuffer;

class Prop : public Entity
{
public:
	Prop(Game* owner, Vec3 const& startPos);
	Prop(CPUMesh* cpuMesh, Material* material);
	~Prop();
	virtual void Update() override;
	virtual void Render() const override;

public:
	std::vector<Vertex_PCU> m_vertexes;
	CPUMesh*				m_cpuMesh = nullptr;
	GPUMesh*				m_gpuMesh = nullptr;
	Material*				m_material = nullptr;
	Texture*				m_texture;
	VertexBuffer*			m_debug = nullptr;
};