#include "Game/SpriteAnimGroupDefinition.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/SpriteDefinition.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/EulerAngles.hpp"
#include "Engine/Math/Mat44.hpp"
#include "Engine/Core/Image.hpp"

SpriteAnimGroupDefinition::SpriteAnimGroupDefinition()
{
}

SpriteAnimGroupDefinition::SpriteAnimGroupDefinition(XmlElement& spriteAnimGroupDefElement, SpriteSheet const& sheet)
{
	m_name					= ParseXmlAttribute(spriteAnimGroupDefElement, "name", m_name);
	m_scaleBySpeed			= ParseXmlAttribute(spriteAnimGroupDefElement, "scaleBySpeed", m_scaleBySpeed);
	m_secondsPerFrame		= ParseXmlAttribute(spriteAnimGroupDefElement, "secondsPerFrame", m_secondsPerFrame);
	std::string playMode	= ParseXmlAttribute(spriteAnimGroupDefElement, "playbackMode", "Once");


	if (playMode == "Once") {
		m_playbackType = SpriteAnimPlaybackType::ONCE;
	}
	if (playMode == "Loop") {
		m_playbackType = SpriteAnimPlaybackType::LOOP;
	}
	if (playMode == "Pingpong") {
		m_playbackType = SpriteAnimPlaybackType::PINGPONG;
	}

	XmlElement* directionDefElement = spriteAnimGroupDefElement.FirstChildElement();
	int frameCount = 0;
	while (directionDefElement) {
		Vec3 direction;
		direction = ParseXmlAttribute(*directionDefElement, "vector", direction);
		direction.Normalize();
		XmlElement* animationDefElement = directionDefElement->FirstChildElement();
		SpriteAnimDefinition animation = SpriteAnimDefinition(*animationDefElement, sheet, m_secondsPerFrame, m_playbackType);
		frameCount = animation.GetFrameNum();
		m_directions.push_back(direction);
		m_animations.push_back(animation);
		//m_animations.emplace(direction, animation);
		directionDefElement = directionDefElement->NextSiblingElement();
	}
	m_duration = m_secondsPerFrame * (float)frameCount;
}

SpriteAnimGroupDefinition::SpriteAnimGroupDefinition(XmlElement& spriteAnimGroupDefElement)
{
	m_name = ParseXmlAttribute(spriteAnimGroupDefElement, "name", m_name);
	m_scaleBySpeed = ParseXmlAttribute(spriteAnimGroupDefElement, "scaleBySpeed", m_scaleBySpeed);
	m_secondsPerFrame = ParseXmlAttribute(spriteAnimGroupDefElement, "secondsPerFrame", m_secondsPerFrame);
	std::string playMode = ParseXmlAttribute(spriteAnimGroupDefElement, "playbackMode", "Once");

	if (playMode == "Once") {
		m_playbackType = SpriteAnimPlaybackType::ONCE;
	}
	if (playMode == "Loop") {
		m_playbackType = SpriteAnimPlaybackType::LOOP;
	}
	if (playMode == "Pingpong") {
		m_playbackType = SpriteAnimPlaybackType::PINGPONG;
	}

	std::string shader = ParseXmlAttribute(spriteAnimGroupDefElement, "shader", "");
	if (shader != "") {
		m_shader = g_theRenderer->CreateShader(shader.c_str());
	}

	// create sprite sheet
	std::string sheet = ParseXmlAttribute(spriteAnimGroupDefElement, "spriteSheet", sheet);
	IntVec2 cellCount = ParseXmlAttribute(spriteAnimGroupDefElement, "cellCount", cellCount);

	Image image = Image(sheet.c_str());
	Texture* texture = g_theRenderer->CreateTextureFromImage(image);

	SpriteSheet* spriteSheet = new SpriteSheet(*texture, cellCount);

	int startFrame = ParseXmlAttribute(spriteAnimGroupDefElement, "startFrame", 0);
	int endFrame = ParseXmlAttribute(spriteAnimGroupDefElement, "endFrame", 0);
	
	SpriteAnimDefinition animation = SpriteAnimDefinition(*spriteSheet, startFrame, endFrame, 1.f / m_secondsPerFrame, m_playbackType);
	int frameCount = animation.GetFrameNum();
	m_animations.push_back(animation);
	m_duration = (float)frameCount * m_secondsPerFrame;
}

SpriteDefinition const& SpriteAnimGroupDefinition::GetSpriteDefAtTimeAndDirection(float seconds, EulerAngles orientation, Vec3 direction) const
{
	// TODO: insert return statement here
	float maxValue = -999.f;
	int animeIndex = 0;
	for (int directionIndex = 0; directionIndex < m_directions.size(); directionIndex++) {
		float temp = DotProduct3D(direction, orientation.GetMatrix_XFwd_YLeft_ZUp().TransformVectorQuantity3D(m_directions[directionIndex]));
		if (temp > maxValue) {
			maxValue = temp;
			animeIndex = directionIndex;
		}
	}
	//if (animeIndex < m_animations.size())
	return m_animations[animeIndex].GetSpriteDefAtTime(seconds);
}

float SpriteAnimGroupDefinition::GetDuration() const
{
	return m_duration;
}

