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

class Model
{
public:
	Model(CPUMesh* cpuMesh, GPUMesh* gpuMesh, Shader* shader);
	Model(const char* docFilePath);
	~Model();

	void Render() const;

public:
	std::string	m_name;
	std::string m_path;
	Rgba8		m_color = Rgba8::WHITE;
	CPUMesh*	m_cpuMesh = nullptr;
	GPUMesh*	m_gpuMesh = nullptr;
	Shader*		m_shader = nullptr;
	Mat44		m_matrix;
};