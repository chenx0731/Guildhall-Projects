#include "Game/Aries.hpp"
#include "Game/Map.hpp"
#include "Game/Player.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Core/NamedStrings.hpp"
#include "Engine/Core/EngineCommon.hpp"

Aries::Aries(EntityType type, EntityFaction faction, Map* owner, Vec2 const& startPos, float orientationDegrees):Entity(owner, startPos, orientationDegrees)
{
	m_entityType = type;
	m_faction = faction;

	m_physicsRadius		= g_gameConfigBlackboard.GetValue("entityPhysicsRadius", 0.3f);
	m_cosmeticRadius	= g_gameConfigBlackboard.GetValue("entityCosmeticRadius", 0.5f);
	m_textureRadius		= g_gameConfigBlackboard.GetValue("entityTextureRadius", 0.5f);
	m_speed				= g_gameConfigBlackboard.GetValue("ariesDriveSpeed", 0.5f);
	m_angularVelocity	= g_gameConfigBlackboard.GetValue("ariesTurnRate", 180.f);
	m_health			= g_gameConfigBlackboard.GetValue("entityHealth", 5);
	m_maxHealth			= m_health;

	m_isPushedByWall = true;
	m_isPushedByEntities = true;
	m_doesPushEntities = true;
	m_isHitByBullets = true;

	m_distanceMap = TileHeatMap(m_map->GetMapDimension());
	m_goalPosition = m_position;
	
}

Aries::~Aries()
{
}

void Aries::Update(float deltaSeconds)
{
	Entity* const player = g_theGame->m_player;
	bool isSeen = false;
	if (player != nullptr) {
		isSeen = m_map->HasLineOfSight(m_position, player->m_position, player->m_physicsRadius);
	}
	if (isSeen) {
		UpdateWalkpath(true, player->m_position);
	}
	else {
		UpdateWalkpath(false, Vec2(0.f, 0.f));
	}
	if (m_pathWaypoints.size() > 0) {
		Vec2 nextPos = m_pathWaypoints.back();
		m_goalOrientation = (nextPos - m_position).GetOrientationDegrees();
		m_orientationDegrees = GetTurnedTowardDegrees(m_orientationDegrees, m_goalOrientation, m_angularVelocity * deltaSeconds);
		float diffAngle = GetShortestAngularDispDegrees(m_orientationDegrees, m_goalOrientation);
		if (diffAngle < 45.f && diffAngle > -45.f) {
			m_velocity = Vec2::MakeFromPolarDegrees(m_orientationDegrees, m_speed);
			m_position += m_velocity * deltaSeconds;
		}
	}
}

void Aries::Render() const
{
	// Tank
//Texture* tankBaseTexture = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/PlayerTankBase.png");
	std::vector<Vertex_PCU> ariesVerts;
	AABB2 localBounds = AABB2(Vec2(-m_textureRadius, -m_textureRadius), Vec2(m_textureRadius, m_textureRadius));
	AddVertsForAABB2D(ariesVerts, localBounds, Rgba8(255, 255, 255));
	TransformVertexArrayXY3D((int)ariesVerts.size(), ariesVerts.data(), 1.0f, m_orientationDegrees, m_position);
	g_theRenderer->BindTexture(g_textureLibrary[TEXTURE_ENEMY_ARIES]);
	g_theRenderer->DrawVertexArray((int)ariesVerts.size(), ariesVerts.data());

	g_theRenderer->BindTexture(nullptr);
}

void Aries::DebugRender() const
{
	for (int i = 0; i < m_pathWaypoints.size(); i++) {
		if (i == 0) {
			DrawCircle(m_pathWaypoints[i], 0.1f, Rgba8::RED);
			continue;
		}
		DrawCircle(m_pathWaypoints[i], 0.1f, Rgba8::BLUE);
		DebugDrawLine(m_pathWaypoints[i - 1], m_pathWaypoints[i], 0.05f, Rgba8::GREY);
	}
	Entity::DebugRender();
}

void Aries::ReactToBulletImpact(Entity* bullet)
{
 	if (IsPointInsideOrientedSector2D(bullet->m_position, m_position, m_orientationDegrees, 90.f, m_cosmeticRadius)) {
		PushDiscOutOfFixedDisc2D(bullet->m_position, m_physicsRadius, m_position, m_physicsRadius);
		Vec2 bounceFormal = bullet->m_position - m_position;
		bounceFormal.Normalize();
		bullet->m_velocity.Reflect(bounceFormal);
		return;
	}
	PlayMusic(MUSIC_ENEMY_HIT);
	Entity::ReactToBulletImpact(bullet);
}

void Aries::Die()
{
	PlayMusic(MUSIC_ENEMY_DIE);
	Entity::Die();
	Entity* explosion = m_map->SpawnNewEntity(ENTITY_TYPE_EVIL_EXPLOSION, m_position, 0.f);
	m_map->AddEntityToMap(explosion);
}

