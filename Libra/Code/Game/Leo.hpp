#pragma once
#include "Game/Entity.hpp"

class Leo : public Entity
{
public :
	Leo(EntityType type, EntityFaction faction, Map* owner, Vec2 const& startPos, float orientationDegrees);
	~Leo();
	virtual void Update(float deltaSeconds) override;
	virtual void Render() const override;
	void DebugRender() const;
	void FireBullet();
	void Die();
	void ReactToBulletImpact(Entity* bullet);

private:
	float m_goalOrientation = 0.f;
	float m_timer = 0.f;
	float m_fireColdTime = 0.f;
};