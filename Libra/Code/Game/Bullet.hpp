#pragma once
#include "Game/Entity.hpp"

class Bullet : public Entity
{
public :
	Bullet(EntityType type, EntityFaction faction, Map* owner, Vec2 const& startPos, float orientationDegrees);
	~Bullet();
	virtual void Update(float deltaSeconds) override;
	virtual void Render() const override;
	void Die();

	bool CheckOverlapOfWalls(Vec2 &pos);
	void CheckOverlapOfEntities();
	void CheckOverlapOfEntity(Entity* entity);
private:
	int m_bounceTimes = 0;
};