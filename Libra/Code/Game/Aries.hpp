#pragma once
#include "Game/Entity.hpp"

class Aries : public Entity
{
public :
	Aries(EntityType type, EntityFaction faction, Map* owner, Vec2 const& startPos, float orientationDegrees);
	~Aries();
	virtual void Update(float deltaSeconds) override;
	virtual void Render() const override;
	void DebugRender() const;
	void ReactToBulletImpact(Entity* bullet);
	void Die();
private:
	float m_goalOrientation = 0.f;
	float m_timer = 0.f;
};