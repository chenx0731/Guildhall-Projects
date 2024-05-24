#include "Game/Asteroid.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Audio/AudioSystem.hpp"

Asteroid::Asteroid(Game* owner, Vec2 const& startPos)
	:Entity(owner, startPos)
{
	m_physicsRadius = ASTEROID_PHYSICS_RADIUS;
	m_cosmeticRadius = ASTEROID_COSMETIC_RADIUS;
	InitializeLocalVerts();
	m_health = ASTEROID_HEALTH;
}

Asteroid::~Asteroid()
{
}

void Asteroid::Update()
{
	m_position += (m_velocity * m_game->m_clock->GetDeltaSeconds());
	m_orientationDegrees += (m_angularVelocity * m_game->m_clock->GetDeltaSeconds());
	if (m_health == 0)
	{
		m_isDead = true;
		m_isGarbage = true;
	}
	if (IsOffscreen())
	{
		if (m_position.x < -m_cosmeticRadius)
		{
			m_position.x = m_cosmeticRadius + WORLD_SIZE_X;
		}
		if (m_position.x > m_cosmeticRadius + WORLD_SIZE_X)
		{
			m_position.x = -m_cosmeticRadius;
		}
		if (m_position.y < -m_cosmeticRadius)
		{
			m_position.y = m_cosmeticRadius + WORLD_SIZE_Y;
		}
		if (m_position.y > m_cosmeticRadius + WORLD_SIZE_Y)
		{
			m_position.y = -m_cosmeticRadius;
		}
	}
}

void Asteroid::Render() const
{
	Vertex_PCU tempAsteroidVerts[NUM_ASTEROID_VERTS];
	for (int vertIndex = 0; vertIndex < NUM_ASTEROID_VERTS; vertIndex++)
	{
		tempAsteroidVerts[vertIndex] = m_localVerts[vertIndex];
	}

	TransformVertexArrayXY3D(NUM_ASTEROID_VERTS, tempAsteroidVerts, 1.f, m_orientationDegrees, m_position);
	g_theRenderer->DrawVertexArray(NUM_ASTEROID_VERTS, tempAsteroidVerts);
}

void Asteroid::Die()
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

void Asteroid::Damege(Vec2 position)
{
	Damege();
	int debrisNum = g_rng->RollRandomIntInRange(DEBRIS_ENTITY_DAMEGE_MIN, DEBRIS_ENTITY_DAMEGE_MAX);
	for (int debrisIndex = 0; debrisIndex < debrisNum; debrisIndex++)
	{
		m_game->SpawnRandomDebris(position, 0.2f, 0.5f, m_color, 5.f, 10.f);
	}
}

void Asteroid::Damege()
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

void Asteroid::InitializeLocalVerts()
{
	float asteroidRadii[NUM_ASTEROID_SIDES] = {};
	for (int sideNum = 0; sideNum < NUM_ASTEROID_SIDES; sideNum++)
	{
		asteroidRadii[sideNum] = g_rng->RollRandomFloatInRange(m_physicsRadius, m_cosmeticRadius);
	}

	constexpr float degreesPerAsteroidSide = 360.f / (float)NUM_ASTEROID_SIDES;
	Vec2 asteroidLocalVertPositions[NUM_ASTEROID_SIDES] = {};
	for (int sideNum = 0; sideNum < NUM_ASTEROID_SIDES; sideNum++)
	{
		float degrees = degreesPerAsteroidSide * (float)sideNum;
		float radius = asteroidRadii[sideNum];
		asteroidLocalVertPositions[sideNum].x = radius * CosDegrees(degrees);
		asteroidLocalVertPositions[sideNum].y = radius * SinDegrees(degrees);
	}

	for (int triNum = 0; triNum < NUM_ASTEROID_TRIS; triNum++)
	{
		int startRadiusIndex = triNum;
		int endRadiusIndex = (triNum + 1) % NUM_ASTEROID_SIDES;
		int firstVertIndex = (triNum * 3) + 0;
		int secondVertIndex = (triNum * 3) + 1;
		int thirdVertIndex = (triNum * 3) + 2;
		Vec2 secondVertOfs = asteroidLocalVertPositions[startRadiusIndex];
		Vec2 thirdVertOfs = asteroidLocalVertPositions[endRadiusIndex];
		m_localVerts[firstVertIndex].m_position = Vec3(0.f, 0.f, 0.f);
		m_localVerts[secondVertIndex].m_position = Vec3(secondVertOfs.x, secondVertOfs.y, 0.f);
		m_localVerts[thirdVertIndex].m_position = Vec3(thirdVertOfs.x, thirdVertOfs.y, 0.f);
	}

	for (int vertIndex = 0; vertIndex < NUM_ASTEROID_VERTS; vertIndex++)
	{
		m_localVerts[vertIndex].m_color = m_color;
	}
}