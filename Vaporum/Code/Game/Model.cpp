#include "Game/Model.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Renderer/GPUMesh.hpp"
#include "Engine/Renderer/CPUMesh.hpp"
#include "Engine/Renderer/Shader.hpp"
#include "Engine/Core/XmlUtils.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Core/OBJLoader.hpp"
#include "Engine/Core/FileUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

Model::Model(CPUMesh* cpuMesh, GPUMesh* gpuMesh, Shader* shader) : m_cpuMesh (cpuMesh), m_gpuMesh(gpuMesh), m_shader(shader)
{

}

Model::Model(const char* docFilePath)
{
	//std::string file;
	//FileReadToString(file, docFilePath);
	XmlDocument modelDefDoc;
	bool isFileValid = IsFileExist(docFilePath);
	if (!isFileValid) {
		ERROR_AND_DIE(Stringf("Can't open file: %s", docFilePath));
	}
	modelDefDoc.LoadFile(docFilePath);
	XmlElement* modelDefElement = modelDefDoc.RootElement();
	
	//XmlElement* modelDefElement = rootElement->FirstChildElement();
	
	m_name = ParseXmlAttribute(*modelDefElement, "name", "");
	m_path = ParseXmlAttribute(*modelDefElement, "path", "");;

	std::string shaderPath = ParseXmlAttribute(*modelDefElement, "shader", "Default");
	m_shader = g_theRenderer->CreateShader(shaderPath.c_str());

	modelDefElement = modelDefElement->FirstChildElement();
	
	Vec3 x, y, z, t;
	float scale;
	x = ParseXmlAttribute(*modelDefElement, "x", Vec3());
	y = ParseXmlAttribute(*modelDefElement, "y", Vec3());
	z = ParseXmlAttribute(*modelDefElement, "z", Vec3());
	t = ParseXmlAttribute(*modelDefElement, "t", Vec3());
	scale = ParseXmlAttribute(*modelDefElement, "scale", 1.f);

	Mat44 mat(x, y, z, t);
	mat.AppendScaleUniform3D(scale);
	m_matrix = mat;
	

	std::vector<Vertex_PCUTBN> vertexes;
	std::vector<unsigned int> indexes;

	LoadOBJByFileName(m_path.c_str(), mat, vertexes, indexes);

	m_cpuMesh = new CPUMesh(vertexes, indexes);
	m_gpuMesh = g_theRenderer->CreateGPUMeshFromCPUMesh(m_cpuMesh);
}

Model::~Model()
{
	delete m_cpuMesh;
	m_cpuMesh = nullptr;
	delete m_gpuMesh;
	m_gpuMesh = nullptr;
}

void Model::Render() const
{
	g_theRenderer->BindShader(m_shader);
	g_theRenderer->SetLightConstants(Vec3(0.5f, 0.5f, -1.f), 0.9f, Rgba8(200, 200, 200));
	g_theRenderer->DrawVertexIndexArray((int)m_cpuMesh->m_vertexes.size(), (int)m_cpuMesh->m_indexes.size(), m_cpuMesh->m_vertexes.data(), m_cpuMesh->m_indexes.data(), m_gpuMesh->m_vertexBuffer, m_gpuMesh->m_indexBuffer);
	g_theRenderer->BindShader(nullptr);
	g_theRenderer->SetModelConstants();
}

