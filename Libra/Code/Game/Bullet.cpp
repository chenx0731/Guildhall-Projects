#include "Game/Bullet.hpp"
#include "Game/Map.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Core/NamedStrings.hpp"
#include "Engine/Core/EngineCommon.hpp"

Bullet::Bullet(EntityType type, EntityFaction faction, Map* owner, Vec2 const& startPos, float orientationDegrees):Entity(owner, startPos, orientationDegrees)
{
	m_entityType = type;
	m_faction = faction;
	m_physicsRadius = 0.f;
	m_cosmeticRadius = 0.f;
	m_textureRadius = g_gameConfigBlackboard.GetValue("bulletTextureRadius", 0.05f);
	m_position = startPos;
	m_orientationDegrees = orientationDegrees;
	//m_angularVelocity = LEO_ANGULAR_VELOCITY;
	m_isPushedByWall = false;
	m_isPushedByEntities = false;
	m_doesPushEntities = false;
	m_isHitByBullets = false;
	if (type == ENTITY_TYPE_GOOD_BOLT || type == ENTITY_TYPE_GOOD_BULLET) {
		m_bounceTimes = g_gameConfigBlackboard.GetValue("playerBulletBouncingTimes", 2);
	}
	if (m_entityType == ENTITY_TYPE_EVIL_BOLT) {
		m_velocity = Vec2::MakeFromPolarDegrees(m_orientationDegrees, g_gameConfigBlackboard.GetValue("defaultBoltSpeed", 6.f));
	}
	if (m_entityType == ENTITY_TYPE_EVIL_BULLET) {
		m_velocity = Vec2::MakeFromPolarDegrees(m_orientationDegrees, g_gameConfigBlackboard.GetValue("defaultBulletSpeed", 3.f));
	}
	if (m_entityType == ENTITY_TYPE_GOOD_BOLT) {
		m_velocity = Vec2::MakeFromPolarDegrees(m_orientationDegrees, g_gameConfigBlackboard.GetValue("defaultBoltSpeed", 6.f));
	}
	if (m_entityType == ENTITY_TYPE_GOOD_BULLET) {
		m_velocity = Vec2::MakeFromPolarDegrees(m_orientationDegrees, g_gameConfigBlackboard.GetValue("defaultBulletSpeed", 3.f));
	}
	if (m_entityType == ENTITY_TYPE_EVIL_GUIDE_BULLET) {
		//m_velocity = Vec2::MakeFromPolarDegrees(m_orientationDegrees, g_gameConfigBlackboard.GetValue("defaultBulletSpeed", 3.f));
		m_speed = g_gameConfigBlackboard.GetValue("defaultBulletSpeed", 3.f);
	}
	Entity* explosion = m_map->SpawnNewEntity(ENTITY_TYPE_BULLET_EXPLOSION, m_position, 0.f);
	m_map->AddEntityToMap(explosion);
}

Bullet::~Bullet()
{
}

void Bullet::Update(float deltaSeconds)
{
	CheckOverlapOfEntities();
	if (m_entityType == ENTITY_TYPE_GOOD_BOLT || m_entityType == ENTITY_TYPE_GOOD_BULLET) {
		Vec2 nextPos = m_position + m_velocity * deltaSeconds;
		if (m_bounceTimes <= 0)
		{
			if (CheckOverlapOfWalls(m_position)) {
				Die();
			}
		}
		if (CheckOverlapOfWalls(nextPos) && m_bounceTimes > 0) {
			IntVec2 now = m_map->GetTileCoordsForWorldPos(m_position);
			IntVec2 next = m_map->GetTileCoordsForWorldPos(nextPos);
			if (now.x != next.x) {
				m_velocity.x = -m_velocity.x;
			}
			if (now.y != next.y) {
				m_velocity.y = -m_velocity.y;
			}
			m_orientationDegrees = m_velocity.GetOrientationDegrees();
			m_bounceTimes--;
		}
		else {
			m_position = nextPos;
		}
	}
	else
	{ 
		if (CheckOverlapOfWalls(m_position)) {
			Die();
		}
		if (m_entityType == ENTITY_TYPE_EVIL_GUIDE_BULLET) {
			m_velocity = g_theGame->m_player->m_position - m_position;
			m_velocity.SetLength(m_speed);
		}
		else m_position += m_velocity * deltaSeconds;
	}
	//CheckOverlapOfWalls(m_position);
	
	
}

void Bullet::Render() const
{
	std::vector<Vertex_PCU> bulletVerts;
	AABB2 localBounds = AABB2(Vec2(-m_textureRadius, -m_textureRadius), Vec2(m_textureRadius, m_textureRadius));
	AddVertsForAABB2D(bulletVerts, localBounds, Rgba8(255, 255, 255));
	TransformVertexArrayXY3D((int)bulletVerts.size(), bulletVerts.data(), 1.0f, m_orientationDegrees, m_position);
	if (m_entityType == ENTITY_TYPE_EVIL_BULLET)
	g_theRenderer->BindTexture(g_textureLibrary[TEXTURE_ENEMY_BULLET]);
	if (m_entityType == ENTITY_TYPE_GOOD_BULLET)
		g_theRenderer->BindTexture(g_textureLibrary[TEXTURE_PLAYER_BULLET]);
	if (m_entityType == ENTITY_TYPE_EVIL_BOLT)
		g_theRenderer->BindTexture(g_textureLibrary[TEXTURE_ENEMY_BOLT]);
	if (m_entityType == ENTITY_TYPE_GOOD_BOLT)
		g_theRenderer->BindTexture(g_textureLibrary[TEXTURE_PLAYER_BOLT]);
	g_theRenderer->DrawVertexArray((int)bulletVerts.size(), bulletVerts.data());

}

void Bullet::Die()
{
	Entity::Die();
}

bool Bullet::CheckOverlapOfWalls(Vec2 &pos)
{
	return m_map->CheckBulletOverlapOfWalls(pos);
}

void Bullet::CheckOverlapOfEntities()
{
	m_map->CheckBulletOverlapOfEntities(this);
}

void Bullet::CheckOverlapOfEntity(Entity* entity)
{
	if (IsPointInsideDisc2D(m_position, entity->m_position, entity->m_physicsRadius)) {
		entity->ReactToBulletImpact(this);
	}
}

