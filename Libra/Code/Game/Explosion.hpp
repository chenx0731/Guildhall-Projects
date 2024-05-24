#pragma once
#include "Game/Entity.hpp"
#include "Engine/Renderer/SpriteAnimDefinition.hpp"

class Explosion : public Entity
{
public:
	Explosion(EntityType type, float lifetime, float size, Map* owner, Vec2 const& startPos, float orientationDegrees);
	~Explosion();

	virtual void Update(float deltaSeconds) override;
	virtual void Render() const override;
public:
	SpriteAnimDefinition*	m_anim;
	float					m_lifeTime;
	float					m_size;
	float					m_livingTime = 0.f;
};