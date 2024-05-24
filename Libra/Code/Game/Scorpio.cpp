#include "Game/Scorpio.hpp"
#include "Game/Map.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/RaycastResult2D.hpp"
#include "Engine/Core/NamedStrings.hpp"
#include "Engine/Core/EngineCommon.hpp"

Scorpio::Scorpio(EntityType type, EntityFaction faction, Map* owner, Vec2 const& startPos, float orientationDegrees):Entity(owner, startPos, orientationDegrees)
{
	m_entityType = type;
	m_faction = faction;

	m_physicsRadius		= g_gameConfigBlackboard.GetValue("entityPhysicsRadius", 0.3f);
	m_cosmeticRadius	= g_gameConfigBlackboard.GetValue("entityCosmeticRadius", 0.5f);
	m_textureRadius		= g_gameConfigBlackboard.GetValue("entityTextureRadius", 0.5f);
	m_angularVelocity	= g_gameConfigBlackboard.GetValue("scorpioTurnRate", 30.f);
	m_health			= g_gameConfigBlackboard.GetValue("entityHealth", 5);
	m_visiableRange		= g_gameConfigBlackboard.GetValue("enemyVisibleRange", 10.f);
	m_maxHealth			= m_health;

	m_isPushedByWall = true;
	m_isPushedByEntities = false;
	m_doesPushEntities = true;
	m_isHitByBullets = true;
}

Scorpio::~Scorpio()
{
}

void Scorpio::Update(float deltaSeconds)
{
	m_fireColdTime += deltaSeconds;
	Entity* const player = g_theGame->m_player;
	bool isSeen = false;
	if (player != nullptr) {
		isSeen = m_map->HasLineOfSight(m_position, player->m_position, player->m_physicsRadius);
	}
	//bool isSeen = m_map->HasLineOfSight(m_position, player->m_position, player->m_physicsRadius);
	if (isSeen) {
		float goalOrientation = (player->m_position - m_position).GetOrientationDegrees();
		m_turretOrientation = GetTurnedTowardDegrees(m_turretOrientation, goalOrientation, m_angularVelocity * deltaSeconds);
		float diffAngle = GetShortestAngularDispDegrees(m_turretOrientation, goalOrientation);
		if (diffAngle < 5.f && diffAngle > -5.f) {
			if (m_fireColdTime > 0.3f) {
				FireBullet();
				m_fireColdTime = 0.f;
			}
		}
	}
	else {
		m_turretOrientation += m_angularVelocity * deltaSeconds;
	}
}

void Scorpio::Render() const
{
	RaycastResult2D rr2d = m_map->RaycastVsTiles(m_position, Vec2::MakeFromPolarDegrees(m_turretOrientation), m_visiableRange);
	DrawRay(m_position, m_turretOrientation, rr2d.m_impactDis, 0.05f, Rgba8(255, 0, 0));
	std::vector<Vertex_PCU> tankBaseVerts;
	AABB2 localBounds = AABB2(Vec2(-m_textureRadius, -m_textureRadius), Vec2(m_textureRadius, m_textureRadius));
	AddVertsForAABB2D(tankBaseVerts, localBounds, Rgba8(255, 255, 255));
	TransformVertexArrayXY3D((int)tankBaseVerts.size(), tankBaseVerts.data(), 1.0f, m_orientationDegrees, m_position);
	g_theRenderer->BindTexture(g_textureLibrary[TEXTURE_ENEMY_SCORPIO_BASE]);
	g_theRenderer->DrawVertexArray((int)tankBaseVerts.size(), tankBaseVerts.data());

	std::vector<Vertex_PCU> tankTopVerts;
	AddVertsForAABB2D(tankTopVerts, localBounds, Rgba8(255, 255, 255));
	TransformVertexArrayXY3D((int)tankTopVerts.size(), tankTopVerts.data(), 1.0f, m_turretOrientation, m_position);
	g_theRenderer->BindTexture(g_textureLibrary[TEXTURE_ENEMY_SCORPIO_TOP]);
	g_theRenderer->DrawVertexArray((int)tankTopVerts.size(), tankTopVerts.data());

	g_theRenderer->BindTexture(nullptr);

	
}

void Scorpio::FireBullet()
{
	PlayMusic(MUSIC_ENEMY_SHOOT);
	float bulletOrientation = m_turretOrientation;
	Entity* bullet = m_map->SpawnNewEntity(ENTITY_TYPE_EVIL_BOLT, m_position + m_cosmeticRadius * Vec2::MakeFromPolarDegrees(bulletOrientation), bulletOrientation);
	m_map->AddEntityToMap(bullet);
}

void Scorpio::Die()
{
	PlayMusic(MUSIC_ENEMY_DIE);
	Entity::Die();
	Entity* explosion = m_map->SpawnNewEntity(ENTITY_TYPE_EVIL_EXPLOSION, m_position, 0.f);
	m_map->AddEntityToMap(explosion);
}

void Scorpio::ReactToBulletImpact(Entity* bullet)
{
	PlayMusic(MUSIC_ENEMY_HIT);
	Entity::ReactToBulletImpact(bullet);
}

