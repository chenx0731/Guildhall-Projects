#pragma once
#include "Engine/Core/Rgba8.hpp"
#include <string>
#include <vector>

class Shader;
class Texture;

enum class VertexType
{
	PCUTBN,
	PNCU,
	PCU
};

class Material
{
public:
	Material();
	Material(const char* filepath);
	~Material();

public:
	//Renderer* m_renderer = nullptr;

	std::string m_name;
	Shader* m_shader = nullptr;
	VertexType m_vertexType = VertexType::PCUTBN;
	Texture* m_diffuseTexture = nullptr;
	Texture* m_normalTexture = nullptr;
	Texture* m_specGlossEmitTexture = nullptr;
	Rgba8 m_color = Rgba8::WHITE;
};


