#pragma once
#include "Game/Game.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Core/HeatMaps.hpp"
typedef std::vector<Entity*> EntityList;

enum EntityFaction
{
	FACTION_NULL = -1,
	
	FACTION_GOOD,
	FACTION_NEUTRAL,
	FACTION_EVIL,

	NUM_FACTION
};

enum EntityType
{
	ENTITY_TYPE_NULL = -1,

	ENTITY_TYPE_GOOD_PLAYER,
	ENTITY_TYPE_EVIL_SCORPIO,
	ENTITY_TYPE_EVIL_LEO,
	ENTITY_TYPE_EVIL_ARIES,
	ENTITY_TYPE_EVIL_CAPRICORN,

	ENTITY_TYPE_GOOD_BOLT,
	ENTITY_TYPE_EVIL_BOLT,
	ENTITY_TYPE_GOOD_BULLET,
	ENTITY_TYPE_EVIL_BULLET,
	ENTITY_TYPE_EVIL_GUIDE_BULLET,

	ENTITY_TYPE_GOOD_EXPLOSION,
	ENTITY_TYPE_EVIL_EXPLOSION,
	ENTITY_TYPE_BULLET_EXPLOSION,

	ENTITY_TYPE_HEALTH_BAR,

	NUM_ENTITY_TYPES
};

class Entity
{
public:
	Entity(Map* owner, Vec2 const& startPos, float orientationDegrees);
	Entity(EntityType type, EntityFaction faction, Map* owner, Vec2 const& startPos, float orientationDegrees);
	virtual ~Entity();

	virtual void Update(float deltaSeconds) = 0;
	virtual void Render() const = 0;
	virtual void DebugRender() const;
	virtual void Die();
	virtual void FireBullet();
	virtual void ReactToBulletImpact(Entity* bullet);
	virtual void UpdateWalkpath(bool isSeen, Vec2 goalPosition);
	virtual void AddVertsForHealthBar(std::vector<Vertex_PCU>& verts) const;

	bool IsOffscreen() const;
	Vec2 GetForwardNormal() const;

public:
	Map*	m_map;
	Vec2	m_position;
	Vec2	m_velocity;
	float	m_speed = 0.f;
	float	m_orientationDegrees = 0.f;
	float	m_angularVelocity = 0.f;
	float	m_physicsRadius = 5.f;
	float	m_cosmeticRadius = 10.f;
	float	m_textureRadius = 0.f;
	int		m_health = 1;
	int		m_maxHealth = 1;

	bool	m_canSwim = false;

	bool	m_isDead = false;
	bool	m_isGarbage = false;
	bool	m_isPushedByEntities = false;
	bool	m_doesPushEntities = false;

	bool	m_isPushedByWall = false;
	bool	m_isHitByBullets = false;

	EntityFaction m_faction;
	EntityType m_entityType;

	Vec2				m_goalPosition;
	std::vector<Vec2>	m_pathWaypoints;
	TileHeatMap			m_distanceMap;
	
private:

};