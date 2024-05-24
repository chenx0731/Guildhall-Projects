#pragma once
#include "Game/Game.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Core/VertexUtils.hpp"

class Entity
{
public:
	Entity(Game* owner, Vec2 const& startPos);
	virtual ~Entity();

	virtual void Update() = 0;
	virtual void Render() const = 0;
	virtual void DebugRender() const;
	virtual void Die();

	bool IsOffscreen() const;
	Vec2 GetForwardNormal() const;

public:
	Game* m_game = nullptr;
	Vec2 m_position;
	Vec2 m_velocity;
	float m_orientationDegrees = 0.f;
	float m_angularVelocity = 0.f;
	float m_physicsRadius = 5.f;
	float m_cosmeticRadius = 10.f;
	int m_health = 1;
	bool m_isDead = false;
	bool m_isGarbage = false;
	
private:

};