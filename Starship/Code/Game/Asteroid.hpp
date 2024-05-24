#pragma once
#include "Game/Entity.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Core/Vertex_PCU.hpp"

constexpr int NUM_ASTEROID_SIDES = 16;
constexpr int NUM_ASTEROID_TRIS = NUM_ASTEROID_SIDES;
constexpr int NUM_ASTEROID_VERTS = 3 * NUM_ASTEROID_TRIS;

class Asteroid : public Entity
{
public:
	Asteroid(Game* owner, Vec2 const& startPos);
	~Asteroid();

	virtual void Update() override;
	virtual void Render() const override;
	virtual void Die() override;
	void Damege();
	void Damege(Vec2 position);

private:
	void InitializeLocalVerts();

private:
	Vertex_PCU m_localVerts[NUM_ASTEROID_VERTS];
	Rgba8 m_color = Rgba8(ASTEROID_COLOR_R, ASTEROID_COLOR_G, ASTEROID_COLOR_B);
};