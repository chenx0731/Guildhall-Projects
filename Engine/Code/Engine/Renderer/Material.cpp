#include "Engine/Renderer/Material.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Core/XmlUtils.hpp"
#include "Engine/Core/FileUtils.hpp"
#include "Engine/Renderer/Shader.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/Image.hpp"
#include <d3d11.h>


Material::Material()
{
}

Material::Material(const char* filepath)
{
	XmlDocument materialDefDoc;
	bool isFileValid = IsFileExist(filepath);
	if (!isFileValid) {
		ERROR_AND_DIE(Stringf("Can't open file: %s", filepath));
	}
	materialDefDoc.LoadFile(filepath);
	XmlElement* materialDefElement = materialDefDoc.RootElement();

	m_name = ParseXmlAttribute(*materialDefElement, "name", m_name);

	std::string shader = ParseXmlAttribute(*materialDefElement, "shader", "");
	ShaderConfig config;
	config.m_name = shader;
	config.m_hasVS = true;
	config.m_hasPS = true;
	m_shader = g_theRenderer->CreateShaderByConfig(config);

	std::string vertexType = ParseXmlAttribute(*materialDefElement, "vertexType", "Vertex_PCUTBN");
	if (vertexType == "Vertex_PCUTBN") {
		m_vertexType = VertexType::PCUTBN;
	}
	if (vertexType == "Vertex_PCU") {
		m_vertexType = VertexType::PCU;
	}
	
	std::string diffuseTexture = ParseXmlAttribute(*materialDefElement, "diffuseTexture", "");
	Image diffuse = Image(diffuseTexture.c_str());
	m_diffuseTexture = g_theRenderer->CreateTextureFromImage(diffuse);

	std::string normalTexture = ParseXmlAttribute(*materialDefElement, "normalTexture", "");
	Image normal = Image(normalTexture.c_str());
	m_normalTexture = g_theRenderer->CreateTextureFromImage(normal);

	std::string specGlossEmitTexture = ParseXmlAttribute(*materialDefElement, "specGlossEmitTexture", "");
	Image specGlossEmit = Image(specGlossEmitTexture.c_str());
	m_specGlossEmitTexture = g_theRenderer->CreateTextureFromImage(specGlossEmit);

	m_color = ParseXmlAttribute(*materialDefElement, "color", Rgba8::WHITE);
}

Material::~Material()
{
	
}



