#include "Game/Entity.hpp"
#include "Game/Map.hpp"
#include "Engine/Math/RaycastResult2D.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/NamedStrings.hpp"
#include "Engine/Core/EngineCommon.hpp"


Entity::Entity(Map* owner, Vec2 const& startPos, float orientationDegrees)
{
	m_map = owner;
	m_position = startPos;
	m_orientationDegrees = orientationDegrees;
}

Entity::Entity(EntityType type, EntityFaction faction, Map* owner, Vec2 const& startPos, float orientationDegrees)
{
	m_entityType = type;
	m_faction = faction;
	m_map = owner;
	m_position = startPos;
	m_orientationDegrees = orientationDegrees;
}

Entity::~Entity()
{
}

void Entity::DebugRender() const
{
	float thickness = g_gameConfigBlackboard.GetValue("debugDrawLineThickness", 0.05f);
	// forward
	DebugDrawLine(m_position, m_orientationDegrees, m_cosmeticRadius, thickness, Rgba8(255, 0, 0));
	// relative-left
	DebugDrawLine(m_position, m_orientationDegrees + 90.f, m_cosmeticRadius, thickness, Rgba8(0, 255, 0));
	// outer Ring
	DebugDrawRing(m_position, m_cosmeticRadius, thickness, Rgba8(255, 0, 255));
	// inner Ring
	DebugDrawRing(m_position, m_physicsRadius, thickness, Rgba8(0, 255, 255));
	// m_velocity
	DebugDrawLine(m_position, m_position + m_velocity, thickness, Rgba8(255, 255, 0));
}

void Entity::Die()
{
	m_isDead = true;
	m_isGarbage = true;
}

void Entity::FireBullet()
{
}

void Entity::ReactToBulletImpact(Entity* bullet)
{
	if (g_theGame->m_gameStatus == GAME_STATUS_HANGING)
		return;
	m_health--;
	bullet->Die();
	if (m_health <= 0) {
		Die();
	}
}

void Entity::UpdateWalkpath(bool isSeen, Vec2 goalPosition)
{
	if (isSeen && m_map->GetTileCoordsForWorldPos(goalPosition) != m_map->GetTileCoordsForWorldPos(m_goalPosition)) {
		m_pathWaypoints.clear();
		m_goalPosition = goalPosition;
		IntVec2 goalTile = m_map->GetTileCoordsForWorldPos(m_goalPosition);
		m_map->PopulateDistanceField(m_distanceMap, goalTile, 9999.f, true, true);
		IntVec2 myPosTile = m_map->GetTileCoordsForWorldPos(m_position);
		m_pathWaypoints = m_distanceMap.GeneratePath(myPosTile, goalTile);
	}
	if (IsPointInsideDisc2D(m_goalPosition, m_position, m_physicsRadius) || m_pathWaypoints.empty()) {
		m_pathWaypoints.clear();
		m_goalPosition = m_map->GetRandomWalkableTile();
		IntVec2 goalTile = m_map->GetTileCoordsForWorldPos(m_goalPosition);
		m_map->PopulateDistanceField(m_distanceMap, goalTile, 9999.f, true, true);
		IntVec2 myPosTile = m_map->GetTileCoordsForWorldPos(m_position);
		m_pathWaypoints = m_distanceMap.GeneratePath(myPosTile, goalTile);
	}

	if (m_pathWaypoints.size() >= 2) {
		Vec2 rayCastPos = m_pathWaypoints[m_pathWaypoints.size() - 2];

		Vec2 shoulder = m_physicsRadius * Vec2::MakeFromPolarDegrees(m_orientationDegrees + 90.f);
		RaycastResult2D result = m_map->RaycastVsTiles(m_position, rayCastPos - m_position, (m_position - rayCastPos).GetLength());
		RaycastResult2D result1 = m_map->RaycastVsTiles(m_position - shoulder, rayCastPos - m_position, (m_position - rayCastPos).GetLength());
		RaycastResult2D result2 = m_map->RaycastVsTiles(m_position + shoulder, rayCastPos - m_position, (m_position - rayCastPos).GetLength());
		if (!result.m_didImpact && !result1.m_didImpact && !result2.m_didImpact) {
			m_pathWaypoints.pop_back();
		}
	}

	if (m_pathWaypoints.size() > 0) {
		if (IsPointInsideDisc2D(m_pathWaypoints.back(), m_position, m_physicsRadius)) {
			m_pathWaypoints.pop_back();
		}
	}
}

void Entity::AddVertsForHealthBar(std::vector<Vertex_PCU>& verts) const
{
	Vec2 top = m_position;
	top.y = top.y + 0.6f;
	Vec2 halfDimension = Vec2(0.25f, 0.05f);
	float health = (float)m_health / (float)m_maxHealth;
	AABB2 fullbar = AABB2(top - halfDimension, top + halfDimension);
	AddVertsForAABB2D(verts, fullbar, Rgba8::RED);
	fullbar.m_maxs.x = fullbar.m_mins.x + 0.5f * health;
	AddVertsForAABB2D(verts, fullbar, Rgba8::GREEN);
}

bool Entity::IsOffscreen() const
{
	return false;
}

Vec2 Entity::GetForwardNormal() const
{
	return m_velocity.GetNormalized();
}
