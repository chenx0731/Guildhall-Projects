#define UNUSED(x) (void)(x);
#include "Game/Debris.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/Time.hpp"

Debris::Debris(Game* owner, Vec2 const& startPos, float maxRadius, float minRadius, Rgba8 color) : Entity(owner, startPos)
{
	m_cosmeticRadius = maxRadius;
	m_physicsRadius = minRadius;
	m_sideNum = g_rng->RollRandomIntInRange(3, 8);
	m_vertsNum = 3 * m_sideNum;
	InitializeLocalVerts(maxRadius, minRadius, color);
}

Debris::~Debris()
{
}

void Debris::Update()
{
	//UNUSED(deltaSeconds);
	m_position += m_velocity * m_game->m_clock->GetDeltaSeconds();
	m_orientationDegrees += m_angularVelocity * m_game->m_clock->GetDeltaSeconds();
	/*static float startTime = GetCurrentTimeSeconds();
	static float lifeTime = 0.f;*/
	//lifeTime = GetCurrentTimeSeconds() - startTime;
	m_lifeTime += m_game->m_clock->GetDeltaSeconds();
	if (m_lifeTime >= DEBRIS_LIFETIME_SECONDS || IsOffscreen())
	{
		Die();
		return;
	}
	for (int vertIndex = 0; vertIndex < m_vertsNum; vertIndex++)
	{
		m_localVerts[vertIndex].m_color.a = static_cast<unsigned char> (127 * (1 - m_lifeTime / DEBRIS_LIFETIME_SECONDS));
	}

}

void Debris::Render() const
{
	Vertex_PCU tempDebrisVerts[NUM_DEBRIS_MAX_VERTS];
	for (int vertIndex = 0; vertIndex < m_vertsNum; vertIndex++)
	{
		tempDebrisVerts[vertIndex] = m_localVerts[vertIndex];
	}
	TransformVertexArrayXY3D(m_vertsNum, tempDebrisVerts, 1.f, m_orientationDegrees, m_position);
	g_theRenderer->DrawVertexArray(m_vertsNum, tempDebrisVerts);
}

void Debris::Die()
{
	m_isDead = true;
	m_isGarbage = true;
}

void Debris::InitializeLocalVerts(float maxRadius, float minRadius, Rgba8 color)
{
	float debrisRadii[NUM_DEBRIS_MAX_TRI] = {};
	float maxR = 0.f;
	float minR = maxRadius;
	for (int sideNum = 0; sideNum < m_sideNum; sideNum++)
	{
		debrisRadii[sideNum] = g_rng->RollRandomFloatInRange(minRadius, maxRadius);
		if (debrisRadii[sideNum] > maxR)
		{
			maxR = debrisRadii[sideNum];
		}
		if (debrisRadii[sideNum] < minR)
		{
			minR = debrisRadii[sideNum];
		}
	}

	m_cosmeticRadius = maxR;
	m_physicsRadius = minR;

	float degreesPerAsteroidSide = 360.f / (float)m_sideNum;
	Vec2 asteroidLocalVertPositions[NUM_DEBRIS_MAX_TRI] = {};
	for (int sideNum = 0; sideNum < m_sideNum; sideNum++)
	{
		float degrees = degreesPerAsteroidSide * (float)sideNum;
		float radius = debrisRadii[sideNum];
		asteroidLocalVertPositions[sideNum].x = radius * CosDegrees(degrees);
		asteroidLocalVertPositions[sideNum].y = radius * SinDegrees(degrees);
	}

	for (int triNum = 0; triNum < m_sideNum; triNum++)
	{
		int startRadiusIndex = triNum;
		int endRadiusIndex = (triNum + 1) % m_sideNum;
		int firstVertIndex = (triNum * 3) + 0;
		int secondVertIndex = (triNum * 3) + 1;
		int thirdVertIndex = (triNum * 3) + 2;
		Vec2 secondVertOfs = asteroidLocalVertPositions[startRadiusIndex];
		Vec2 thirdVertOfs = asteroidLocalVertPositions[endRadiusIndex];
		m_localVerts[firstVertIndex].m_position = Vec3(0.f, 0.f, 0.f);
		m_localVerts[secondVertIndex].m_position = Vec3(secondVertOfs.x, secondVertOfs.y, 0.f);
		m_localVerts[thirdVertIndex].m_position = Vec3(thirdVertOfs.x, thirdVertOfs.y, 0.f);
	}
	color.a = 127;
	for (int vertIndex = 0; vertIndex < m_vertsNum; vertIndex++)
	{
		m_localVerts[vertIndex].m_color = color;
	}
}
