#pragma once
#include "Game/Game.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/Mat44.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Math/EulerAngles.hpp"
#include <string>
class CPUMesh;
class GPUMesh;
class Shader;
class Material;
class VertexBuffer;

class Model
{
public:
	Model(CPUMesh* cpuMesh, GPUMesh* gpuMesh, Material* m_material);
	Model(const char* docFilePath);
	~Model();
	Mat44 GetModelMatrix() const;

	void Update(float deltaSeconds);
	void Render() const;

public:
	std::string	m_name;
	std::string m_path;
	Rgba8		m_color = Rgba8::WHITE;
	CPUMesh*	m_cpuMesh = nullptr;
	GPUMesh*	m_gpuMesh = nullptr;
	VertexBuffer* m_debug = nullptr;
	Material*	m_material = nullptr;
	Mat44		m_matrix;

	EulerAngles m_orientation;
	EulerAngles m_angularVelocity;
};