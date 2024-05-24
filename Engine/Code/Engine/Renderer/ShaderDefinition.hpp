#pragma once
#include "Engine/Math/Vec2.hpp"
#include "Engine/Core/XmlUtils.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/HashedCaseInsensitiveString.hpp"

class Shader;
struct Channel
{
	std::string	m_name;
	float		m_min;
	float		m_max;
	float		m_default;
	float		m_speed;
	bool		m_visibility;
};
class ShaderDefinition
{
public:
	ShaderDefinition(XmlElement& shaderDefElement);

	void WriteIntoXmlDoc(XmlDocument& xmlDoc, XmlElement& xmlEle);

	~ShaderDefinition();

public:
	std::string								m_name;
	std::string								m_filePath;
	Shader*									m_shader = nullptr;
	//std::map<HCIString, Channel>			m_channels;
	std::vector<Channel>					m_channels;
	/*
	std::vector<std::string>				m_channelNames;
	std::vector<float>						m_channelMins;
	std::vector<float>						m_channelMaxs;
	std::vector<float>						m_channelDefault;
	std::vector<float>						m_channelSpeed;
	std::vector<bool>						m_channelVisibility;*/
};