#pragma once
#include "Game/Entity.hpp"

class Player : public Entity
{
public :
	Player(EntityType type, EntityFaction faction, Map* owner, Vec2 const& startPos, float orientationDegrees);
	~Player();
	virtual void Update(float deltaSeconds) override;
	virtual void Render() const override;
	void DebugRender() const;
	void FireBullet();
	void Die();
	void ReactToBulletImpact(Entity* bullet);

private:
	void UpdateFromKeyboard(float deltaSeconds);
	void UpdateFromController(float deltaSeconds);
private:
	Vec2	m_directionKeyboard;
	Vec2	m_directionController;
	float	m_turretOrientation = 0.f;
	Vec2	m_turretDirectionKeyboard;
	
	float	m_turretAngularVelocity = 0.f;
	float	m_orientationGoal = 0.f;
	float	m_turretOrientationGoal = 0.f;
	float	m_fireColdTime = 0.f;
};