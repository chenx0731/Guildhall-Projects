#pragma once
#include "Game/Entity.hpp"

class Scorpio : public Entity
{
public :
	Scorpio(EntityType type, EntityFaction faction, Map* owner, Vec2 const& startPos, float orientationDegrees);
	~Scorpio();
	virtual void Update(float deltaSeconds) override;
	virtual void Render() const override;
	void FireBullet();
	void Die();
	void ReactToBulletImpact(Entity* bullet);
	
private:
	float	m_turretOrientation;
	float	m_fireColdTime = 0.f;
	float	m_visiableRange = 0.f;
};