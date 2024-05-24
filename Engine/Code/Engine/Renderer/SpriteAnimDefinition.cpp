#include "Engine/Renderer/SpriteAnimDefinition.hpp"
#include "Engine/Renderer/SpriteDefinition.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/XmlUtils.hpp"

SpriteAnimDefinition::SpriteAnimDefinition(SpriteSheet const& sheet, int startSpriteIndex, int endSpriteIndex, float framesPerSecond, SpriteAnimPlaybackType playbackType) : m_spriteSheet(sheet)
{
	m_startSpriteIndex = startSpriteIndex;
	m_endSpriteIndex = endSpriteIndex;
	m_secondsPerFrame = 1.f / framesPerSecond;
	m_playbackType = playbackType;
}

SpriteAnimDefinition::SpriteAnimDefinition(XmlElement& spriteAnimDef, SpriteSheet const& sheet, float secondsPerFrame, SpriteAnimPlaybackType playbackType) : m_spriteSheet(sheet)
{
	m_startSpriteIndex	= ParseXmlAttribute(spriteAnimDef, "startFrame", 0);
	m_endSpriteIndex	= ParseXmlAttribute(spriteAnimDef, "endFrame", 0);
	m_secondsPerFrame	= secondsPerFrame;
	m_playbackType		= playbackType;
}

SpriteDefinition const& SpriteAnimDefinition::GetSpriteDefAtTime(float seconds) const
{
	// TODO: insert return statement here
	int frameNum = RoundDownToInt(seconds / m_secondsPerFrame);

	switch (m_playbackType)
	{
	case SpriteAnimPlaybackType::ONCE: {
		if (frameNum >= m_endSpriteIndex - m_startSpriteIndex + 1) {
			return m_spriteSheet.GetSpriteDef(m_endSpriteIndex);
		}
		else {
			return m_spriteSheet.GetSpriteDef(m_startSpriteIndex + frameNum);
		}
	}
		
	case SpriteAnimPlaybackType::LOOP: {
		int index = frameNum % (m_endSpriteIndex - m_startSpriteIndex + 1);
		return m_spriteSheet.GetSpriteDef(m_startSpriteIndex + index);
	}
		
	case SpriteAnimPlaybackType::PINGPONG: {
		int index = frameNum % ((m_endSpriteIndex - m_startSpriteIndex) * 2);
		if (index <= m_endSpriteIndex - m_startSpriteIndex)
			return m_spriteSheet.GetSpriteDef(m_startSpriteIndex + index);
		else return m_spriteSheet.GetSpriteDef(m_endSpriteIndex * 2 - index - m_startSpriteIndex);
	}
	}

	return m_spriteSheet.GetSpriteDef(m_startSpriteIndex);
}

int SpriteAnimDefinition::GetFrameNum() const
{
	return m_endSpriteIndex - m_startSpriteIndex + 1;
}