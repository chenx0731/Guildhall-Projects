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
#include "Engine/Renderer/Material.hpp"
#include "Engine/Renderer/VertexBuffer.hpp"

Model::Model(CPUMesh* cpuMesh, GPUMesh* gpuMesh, Material* material) : m_cpuMesh (cpuMesh), m_gpuMesh(gpuMesh), m_material(material)
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

	std::string materialPath = ParseXmlAttribute(*modelDefElement, "material", "Default");
	m_material = new Material(materialPath.c_str());
		//= g_theRenderer->CreateShader(shaderPath.c_str());

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

	std::vector<Vertex_PCU> vertexesPCU;
	AddVertsForTBNDebug(vertexesPCU, vertexes);

	m_debug = g_theRenderer->CreateVertexBuffer(sizeof(Vertex_PCU), sizeof(Vertex_PCU));
	//m_debug->m_topology = Topology::LineList;
	g_theRenderer->CopyCPUToGPU(vertexesPCU.data(), vertexesPCU.size() * sizeof(Vertex_PCU), sizeof(Vertex_PCU), m_debug);
	//m_debug->m_size = vertexesPCU.size();
}

Model::~Model()
{
	delete m_cpuMesh;
	m_cpuMesh = nullptr;
	delete m_gpuMesh;
	m_gpuMesh = nullptr;
	delete m_debug;
	m_debug = nullptr;
}

Mat44 Model::GetModelMatrix() const
{
	Mat44 mat = m_orientation.GetMatrix_XFwd_YLeft_ZUp();
	return mat;
}

void Model::Update(float deltaSeconds)
{
	m_orientation.m_pitchDegrees += m_angularVelocity.m_pitchDegrees * deltaSeconds;
	m_orientation.m_yawDegrees += m_angularVelocity.m_yawDegrees * deltaSeconds;
	m_orientation.m_rollDegrees += m_angularVelocity.m_rollDegrees * deltaSeconds;
}

void Model::Render() const
{
	g_theRenderer->BindShader(m_material->m_shader);
	g_theRenderer->BindTextures(m_material->m_diffuseTexture, m_material->m_normalTexture, m_material->m_specGlossEmitTexture);
	g_theRenderer->SetModelConstants(GetModelMatrix());
	g_theRenderer->DrawVertexIndexArray((int)m_cpuMesh->m_vertexes.size(), (int)m_cpuMesh->m_indexes.size(), m_cpuMesh->m_vertexes.data(), m_cpuMesh->m_indexes.data(), m_gpuMesh->m_vertexBuffer, m_gpuMesh->m_indexBuffer);
	g_theRenderer->BindShader(nullptr);
	g_theRenderer->BindTextures(nullptr, nullptr, nullptr);

	g_theRenderer->BindTexture(nullptr);
	//g_theRenderer->DrawVertexArray(2, temp.data(), Topology::LineList);
	if (g_theGame->m_isDebug)
		g_theRenderer->DrawVertexBuffer(m_debug, (int)(m_debug->m_size / m_debug->m_stride), Topology::LineList);
}

