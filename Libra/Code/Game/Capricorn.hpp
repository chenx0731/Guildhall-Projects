#pragma once
#include "Game/Entity.hpp"

class Capricorn : public Entity
{
public :
	Capricorn(EntityType type, EntityFaction faction, Map* owner, Vec2 const& startPos, float orientationDegrees);
	~Capricorn();
	virtual void Update(float deltaSeconds) override;
	virtual void Render() const override;
	void DebugRender() const;
	void FireBullet();
	void Die();
	void ReactToBulletImpact(Entity* bullet);
private:
	float m_goalOrientation = 0.f;
	float m_timer = 0.f;
};