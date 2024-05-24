#include "Game/Beetle.hpp"
#include "Game/PlayerShip.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Audio/AudioSystem.hpp"

Beetle::Beetle(Game* owner, Vec2 const& startPos)
	:Entity(owner, startPos)
{
	m_physicsRadius = BEETLE_PHYSICS_RADIUS;
	m_cosmeticRadius = BEETLE_COSMETIC_RADIUS;
	InitializeLocalVerts();
	m_health = BEETLE_HEALTH;
	m_position = startPos;
}

Beetle::~Beetle()
{
}

void Beetle::Update()
{
	m_position += (m_velocity * m_game->m_clock->GetDeltaSeconds());
	m_orientationDegrees = GetOrientationDegree();
	m_velocity = Vec2(BEETLE_SPEED * CosDegrees(m_orientationDegrees), BEETLE_SPEED * SinDegrees(m_orientationDegrees));
	if (m_health == 0)
	{
		m_isDead = true;
		m_isGarbage = true;
	}
}

void Beetle::Render() const
{
	Vertex_PCU tempBeetleVerts[NUM_BEETLE_VERTS];
	for (int vertIndex = 0; vertIndex < NUM_BEETLE_VERTS; vertIndex++)
	{
		tempBeetleVerts[vertIndex] = m_localVerts[vertIndex];
	}

	TransformVertexArrayXY3D(NUM_BEETLE_VERTS, tempBeetleVerts, 1.f, m_orientationDegrees, m_position);
	g_theRenderer->DrawVertexArray(NUM_BEETLE_VERTS, tempBeetleVerts);
}

void Beetle::Die()
{
	SoundID testSound = g_theAudio->CreateOrGetSound("Data/Audio/EnemyHit.wav");
	g_theAudio->StartSound(testSound);

	m_isDead = true;
	m_isGarbage = true;
	int debrisNum = g_rng->RollRandomIntInRange(DEBRIS_ENTITY_DEATH_MIN, DEBRIS_ENTITY_DEATH_MAX);
	for (int debrisIndex = 0; debrisIndex < debrisNum; debrisIndex++)
	{
		m_game->SpawnRandomDebris(m_position, 0.2f, 1.5f, m_color, 2.f, 5.f);
	}
}

void Beetle::Damege()
{
	if (m_health > 0)
	{
		m_health--;
		SoundID testSound = g_theAudio->CreateOrGetSound("Data/Audio/EnemyShoot.wav");
		g_theAudio->StartSound(testSound);
	}
		
	if (m_health == 0)
	{
		Die();
		return;
	}
}

void Beetle::Damege(Vec2 position)
{
	Damege();
	int debrisNum = g_rng->RollRandomIntInRange(DEBRIS_ENTITY_DAMEGE_MIN, DEBRIS_ENTITY_DAMEGE_MAX);
	for (int debrisIndex = 0; debrisIndex < debrisNum; debrisIndex++)
	{
		m_game->SpawnRandomDebris(position, 0.2f, 0.5f, m_color, 5.f, 10.f);
	}
}

void Beetle::InitializeLocalVerts()
{
	m_localVerts[0].m_position = Vec3(1.5f, 1.f, 0.f);
	m_localVerts[2].m_position = Vec3(-1.5f, -1.7f, 0.f);
	m_localVerts[1].m_position = Vec3(-1.3f, 1.2f, 0.f);
	m_localVerts[3].m_position = Vec3(1.5f, 1.f, 0.f);
	m_localVerts[4].m_position = Vec3(-1.5f, -1.7f, 0.f);
	m_localVerts[5].m_position = Vec3(1.3f, -1.1f, 0.f);

	for (int vertIndex = 0; vertIndex < NUM_BEETLE_VERTS; vertIndex++)
	{
		m_localVerts[vertIndex].m_color = m_color;
	}
}

float Beetle::GetOrientationDegree()
{
	if ((m_game->m_playerShip)->m_isDead)
	{
		return m_orientationDegrees;
	}
	float degree = Atan2Degrees((m_game->m_playerShip)->m_position.y - m_position.y, (m_game->m_playerShip)->m_position.x - m_position.x);
	return degree;
}
