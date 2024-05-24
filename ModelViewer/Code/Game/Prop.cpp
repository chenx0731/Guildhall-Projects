#include "Game/Prop.hpp"
#include "Game/Game.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Renderer/CPUMesh.hpp"
#include "Engine/Renderer/GPUMesh.hpp"
#include "Engine/Renderer/Material.hpp"
#include "Engine/Renderer/VertexBuffer.hpp"
#include "Engine/Renderer/IndexBuffer.hpp"


Prop::Prop(Game* owner, Vec3 const& startPos) : Entity(owner, startPos)
{
	m_game = owner;
	m_position = startPos;
}

Prop::Prop(CPUMesh* cpuMesh, Material* material)
{
	m_cpuMesh = cpuMesh;
	m_material = material;
	std::vector<Vertex_PCU> vertexesPCU;
	AddVertsForTBNDebug(vertexesPCU, cpuMesh->m_vertexes);

	VertexBuffer* vb = g_theRenderer->CreateVertexBuffer(sizeof(Vertex_PCUTBN), sizeof(Vertex_PCUTBN));
	IndexBuffer* ib = g_theRenderer->CreateIndexBuffer(sizeof(unsigned int));

	g_theRenderer->CopyCPUToGPU(m_cpuMesh->m_vertexes.data(), m_cpuMesh->m_vertexes.size() * sizeof(Vertex_PCUTBN), sizeof(Vertex_PCUTBN), vb);
	g_theRenderer->CopyCPUToGPU(m_cpuMesh->m_indexes.data(), m_cpuMesh->m_indexes.size() * sizeof(unsigned int), ib);
	m_gpuMesh = new GPUMesh(vb, ib);
	m_debug = g_theRenderer->CreateVertexBuffer(sizeof(Vertex_PCU), sizeof(Vertex_PCU));
	g_theRenderer->CopyCPUToGPU(vertexesPCU.data(), vertexesPCU.size() * sizeof(Vertex_PCU), sizeof(Vertex_PCU), m_debug);
}

Prop::~Prop()
{
	delete m_debug;
	m_debug = nullptr;
	delete m_gpuMesh;
	m_gpuMesh = nullptr;
}

void Prop::Update()
{
	m_orientation.m_pitchDegrees += m_angularVelocity.m_pitchDegrees * g_theGame->m_clock->GetDeltaSeconds();
	m_orientation.m_yawDegrees	 += m_angularVelocity.m_yawDegrees	 * g_theGame->m_clock->GetDeltaSeconds();
	m_orientation.m_rollDegrees  += m_angularVelocity.m_rollDegrees  * g_theGame->m_clock->GetDeltaSeconds();
}

void Prop::Render() const
{
	g_theRenderer->SetModelConstants(GetModelMatrix(), m_color);
	g_theRenderer->SetBlendMode(BlendMode::ALPHA);
	if (m_material) {
		g_theRenderer->BindShader(m_material->m_shader);
		//if (m_material)
		g_theRenderer->BindTextures(m_material->m_diffuseTexture, m_material->m_normalTexture, m_material->m_specGlossEmitTexture);
	}
	//g_theRenderer->BindShader(m_material->m_shader);
	//if (m_material)
		//g_theRenderer->BindTextures(m_material->m_diffuseTexture, m_material->m_normalTexture, m_material->m_specGlossEmitTexture);
	//g_theRenderer->BindTexture(m_texture);
	//TransformVertexArrayXY3D()
	//g_theRenderer->DrawVertexArray((int)m_vertexes.size(), m_vertexes.data());
	//g_theRenderer->DrawVertexIndexArray((int)m_cpuMesh->m_vertexes.size(), (int)m_cpuMesh->m_indexes.size(), m_cpuMesh->m_vertexes.data(),
		//m_cpuMesh->m_indexes.data(), m_gpuMesh->m_vertexBuffer, m_gpuMesh->m_indexBuffer);
	g_theRenderer->DrawVertexIndexBuffer(m_gpuMesh->m_vertexBuffer, m_gpuMesh->m_indexBuffer, m_cpuMesh->m_indexes.size());

	g_theRenderer->BindTextures(nullptr, nullptr, nullptr);
	g_theRenderer->BindShader(nullptr);
	g_theRenderer->BindTexture(nullptr);
	if (g_theGame->m_isDebug) {
		if (m_debug)
			g_theRenderer->DrawVertexBuffer(m_debug, (int)(m_debug->m_size / m_debug->m_stride), Topology::LineList);
	}

}

