#include "Game/Wasp.hpp"
#include "Game/PlayerShip.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Audio/AudioSystem.hpp"

Wasp::Wasp(Game* owner, Vec2 const& startPos)
	:Entity(owner, startPos)
{
	m_physicsRadius = WASP_PHYSICS_RADIUS;
	m_cosmeticRadius = WASP_COSMETIC_RADIUS;
	InitializeLocalVerts();
	m_health = WASP_HEALTH;
	m_orientationDegrees = GetOrientationDegree();
	m_position = startPos;
}

Wasp::~Wasp()
{
}

void Wasp::Update()
{
	m_renderIndex = (m_renderIndex + 1) % 100;
	m_position += (m_velocity * m_game->m_clock->GetDeltaSeconds());
	m_orientationDegrees = GetOrientationDegree();
	m_velocity += m_game->m_clock->GetDeltaSeconds() * Vec2(WASP_ACCELERATION * CosDegrees(m_orientationDegrees), WASP_ACCELERATION * SinDegrees(m_orientationDegrees));
	m_velocity.ClampLength(WASP_SPEED_MAX);
	if (m_health == 0)
	{
		m_isDead = true;
		m_isGarbage = true;
	}
}

void Wasp::Render() const
{
	Vertex_PCU tempAsteroidVerts[NUM_WASP_VERTS];
	for (int vertIndex = 0; vertIndex < NUM_WASP_VERTS; vertIndex++)
	{
		tempAsteroidVerts[vertIndex] = m_localVerts[vertIndex];
	}

	TransformVertexArrayXY3D(NUM_WASP_VERTS, tempAsteroidVerts, 1.f, m_orientationDegrees, m_position);
	g_theRenderer->DrawVertexArray(NUM_WASP_VERTS, tempAsteroidVerts);
}

void Wasp::Die()
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

void Wasp::Damege(Vec2 position)
{
	Damege();
	int debrisNum = g_rng->RollRandomIntInRange(DEBRIS_ENTITY_DAMEGE_MIN, DEBRIS_ENTITY_DAMEGE_MAX);
	for (int debrisIndex = 0; debrisIndex < debrisNum; debrisIndex++)
	{
		m_game->SpawnRandomDebris(position, 0.2f, 0.5f, m_color, 5.f, 10.f);
	}
}

void Wasp::Damege()
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

void Wasp::InitializeLocalVerts()
{
	m_localVerts[0].m_position = Vec3(2.f, 0.f, 0.f);
	m_localVerts[1].m_position = Vec3(0.f, 2.f, 0.f);
	m_localVerts[2].m_position = Vec3(0.f, -2.f, 0.f);

	// Left wing
	m_localVerts[3].m_position = Vec3(0.f, 1.f, 0.f);
	m_localVerts[5].m_position = Vec3(0.f, -1.f, 0.f);
	m_localVerts[4].m_position = Vec3(-2.f, 0.f, 0.f);

	for (int vertIndex = 0; vertIndex < NUM_WASP_VERTS; vertIndex++)
	{
		m_localVerts[vertIndex].m_color = m_color;
	}
}

float Wasp::GetOrientationDegree()
{
	if ((m_game->m_playerShip)->m_isDead)
	{
		return m_orientationDegrees;
	}
	float degree = Atan2Degrees((m_game->m_playerShip)->m_position.y - m_position.y, (m_game->m_playerShip)->m_position.x - m_position.x);
	return degree;
}