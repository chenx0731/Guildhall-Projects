#include "Game/Bullet.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"

Bullet::Bullet(Game* owner, Vec2 const& startPos)
	:Entity(owner, startPos)
{
	m_physicsRadius = BULLET_PHYSICS_RADIUS;
	m_cosmeticRadius = BULLET_COSMETIC_RADIUS;
	InitializeLocalVerts();
}

Bullet::~Bullet()
{
}

void Bullet::Update()
{
	m_position += (m_velocity * m_game->m_clock->GetDeltaSeconds());
	m_lifeTime += m_game->m_clock->GetDeltaSeconds();
	if (m_lifeTime >= BULLET_LIFETIME_SECONDS || IsOffscreen())
	{
		m_isDead = true;
		m_isGarbage = true;
		return;
	}
	if (IsOffscreen() || m_health == 0)
	{
		m_isDead = true;
		m_isGarbage = true;
	}
}

void Bullet::Render() const
{
	Vertex_PCU tempWorldVerts[NUM_BULLET_VERTS];
	for (int vertIndex = 0; vertIndex < NUM_BULLET_VERTS; vertIndex++)
	{
		tempWorldVerts[vertIndex] = m_localVerts[vertIndex];
	}
	TransformVertexArrayXY3D(NUM_BULLET_VERTS, tempWorldVerts, 1.f, m_orientationDegrees, m_position);
	g_theRenderer->DrawVertexArray(NUM_BULLET_VERTS, tempWorldVerts);
}

void Bullet::Die()
{
	m_isDead = true;
	m_isGarbage = true;
	int debrisNum = g_rng->RollRandomIntInRange(DEBRIS_ENTITY_DAMEGE_MIN, DEBRIS_ENTITY_DAMEGE_MAX);
	for (int debrisIndex = 0; debrisIndex < debrisNum; debrisIndex++)
	{
		m_game->SpawnRandomDebris(m_position, 0.2f, 0.3f, m_color, 5.f, 10.f);
	}
}

void Bullet::InitializeLocalVerts()
{
	m_localVerts[0].m_position = Vec3(0.5f, 0.0f, 0.0f);
	m_localVerts[1].m_position = Vec3(0.0f, 0.5f, 0.0f);
	m_localVerts[2].m_position = Vec3(0.0f, -0.5f, 0.0f);
	m_localVerts[0].m_color = Rgba8(255, 255, 255, 255);
	m_localVerts[1].m_color = Rgba8(255, 255, 0, 255);
	m_localVerts[2].m_color = Rgba8(255, 255, 0, 255);

	m_localVerts[3].m_position = Vec3(0.0f, -0.5f, 0.0f);
	m_localVerts[4].m_position = Vec3(0.0f, 0.5f, 0.0f);
	m_localVerts[5].m_position = Vec3(-2.0f, 0.0f, 0.0f);
	m_localVerts[3].m_color = Rgba8(255, 0, 0, 255);
	m_localVerts[4].m_color = Rgba8(255, 0, 0, 255);
	m_localVerts[5].m_color = Rgba8(255, 0, 0, 0);
}
