#include "Engine/Renderer/ShaderDefinition.hpp"
#include "Engine/Renderer/Shader.hpp"
#include "Engine/Core/FileUtils.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Renderer/Renderer.hpp"

ShaderDefinition::ShaderDefinition(XmlElement& shaderDefElement)
{
	m_name = ParseXmlAttribute(shaderDefElement, "name", "");
	std::string shaderPath = ParseXmlAttribute(shaderDefElement, "shader", "");
	m_filePath = shaderPath;
	m_shader = g_theRenderer->CreateShader(shaderPath.c_str());
	
	XmlElement* channel = shaderDefElement.FirstChildElement();
	channel = channel->FirstChildElement();
	m_channels.reserve(16);
	while (channel)
	{
		Channel newChannel;
		newChannel.m_name			= ParseXmlAttribute(*channel, "name", "");
		newChannel.m_min			= ParseXmlAttribute(*channel, "min", 0.f);
		newChannel.m_max			= ParseXmlAttribute(*channel, "max", 0.f);
		newChannel.m_default		= ParseXmlAttribute(*channel, "default", 0.f);
		newChannel.m_speed			= ParseXmlAttribute(*channel, "speed", 0.f);
		newChannel.m_visibility		= ParseXmlAttribute(*channel, "visible", false);
		
		m_channels.push_back(newChannel);
		
		/*
		m_channelNames.push_back(name);
		m_channelMins.push_back(minV);
		m_channelMaxs.push_back(maxV);
		m_channelDefault.push_back(defaultV);
		m_channelSpeed.push_back(speedV);
		m_channelVisibility.push_back(isVisible);*/

		channel = channel->NextSiblingElement();
	}
}

void ShaderDefinition::WriteIntoXmlDoc(XmlDocument& xmlDoc, XmlElement& xmlEle)
{
	XmlElement* element = xmlDoc.NewElement("ShaderDefinition");
	element->SetAttribute("name", m_name.c_str());
	element->SetAttribute("shader", m_filePath.c_str());
	//xmlDoc.InsertFirstChild(element);
	xmlEle.InsertFirstChild(element);
	if (m_channels.size() > 0)
	{
		XmlElement* channels = element->InsertNewChildElement("Channels");
		element->InsertFirstChild(channels);
		for (int i = 0; i < (int)m_channels.size(); i++)
		{
			XmlElement* channel = channels->InsertNewChildElement("Channel");
			channel->SetAttribute("name", m_channels[i].m_name.c_str());
			channel->SetAttribute("min", m_channels[i].m_min);
			channel->SetAttribute("max", m_channels[i].m_max);
			channel->SetAttribute("default", m_channels[i].m_default);
			channel->SetAttribute("speed", m_channels[i].m_speed);
			channel->SetAttribute("visible", m_channels[i].m_visibility);
			channels->InsertEndChild(channel);
		}
		element->InsertEndChild(channels);
	}
	xmlEle.InsertEndChild(element);
	//xmlDoc.InsertEndChild(element);
		//new XmlElement("ShaderDefinition");
}

ShaderDefinition::~ShaderDefinition()
{
	m_shader = nullptr;
}
