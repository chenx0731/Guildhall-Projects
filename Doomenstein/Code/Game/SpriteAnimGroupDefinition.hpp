#pragma once
#include "Engine/Renderer/SpriteAnimDefinition.hpp"
#include "Engine/Core/XmlUtils.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/EulerAngles.hpp"

class Shader;

class SpriteAnimGroupDefinition
{
public:
	SpriteAnimGroupDefinition();
	SpriteAnimGroupDefinition(XmlElement& spriteAnimGroupDefElement, SpriteSheet const& sheet);
	SpriteAnimGroupDefinition(XmlElement& spriteAnimGroupDefElement);
	SpriteDefinition const& GetSpriteDefAtTimeAndDirection(float seconds, EulerAngles forward = EulerAngles(), Vec3 direction = Vec3()) const;
	float GetDuration() const;
public:
	std::string								m_name;
	bool									m_scaleBySpeed = false;
	float									m_secondsPerFrame = 0.05f; // Client specifies FPS in constructor, but we store period = 1/FPS
	float									m_duration = 0.f;
	SpriteAnimPlaybackType					m_playbackType = SpriteAnimPlaybackType::ONCE;
	Shader*									m_shader;
	//std::map<Vec3, SpriteAnimDefinition>	m_animations;
	std::vector<Vec3>						m_directions;
	std::vector<SpriteAnimDefinition>		m_animations;
};
