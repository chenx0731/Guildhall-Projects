#include "Game/Entity.hpp"

Entity::Entity(Game* owner, Vec2 const& startPos)
{
	m_game = owner;
	m_position = startPos;
}

Entity::~Entity()
{
}

void Entity::DebugRender() const
{
	// forward
	DebugDrawLine(m_position, m_orientationDegrees, m_cosmeticRadius, 0.2f, Rgba8(255, 0, 0));
	// relative-left
	DebugDrawLine(m_position, m_orientationDegrees + 90.f, m_cosmeticRadius, 0.2f, Rgba8(0, 255, 0));
	// outer Ring
	DebugDrawRing(m_position, m_cosmeticRadius, 0.2f, Rgba8(255, 0, 255));
	// inner Ring
	DebugDrawRing(m_position, m_physicsRadius, 0.2f, Rgba8(0, 255, 255));
	// m_velocity
	DebugDrawLine(m_position, m_position + m_velocity, 0.2f, Rgba8(255, 255, 0));
}

void Entity::Die()
{
}

bool Entity::IsOffscreen() const
{
	if (m_position.x > WORLD_SIZE_X + m_cosmeticRadius)
		return true;
	if (m_position.y > WORLD_SIZE_Y + m_cosmeticRadius)
		return true;
	if (m_position.x < -m_cosmeticRadius)
		return true;
	if (m_position.y < -m_cosmeticRadius)
		return true;
	return false;
}

Vec2 Entity::GetForwardNormal() const
{
	return m_velocity.GetNormalized();
}
