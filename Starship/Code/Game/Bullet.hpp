#pragma once
#include "Game/Entity.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Core/Vertex_PCU.hpp"

constexpr int NUM_BULLET_TRIS = 2;
constexpr int NUM_BULLET_VERTS = 3 * NUM_BULLET_TRIS;

class Bullet : public Entity
{
public:
	Bullet(Game* owner, Vec2 const& startPos);
	~Bullet();

	virtual void Update() override;
	virtual void Render() const override;
	virtual void Die() override;
private:
	void InitializeLocalVerts();
private:
	Vertex_PCU m_localVerts[NUM_BULLET_VERTS];
	Rgba8 m_color = Rgba8(BULLET_COLOR_R, BULLET_COLOR_G, BULLET_COLOR_B);
	float m_lifeTime = 0.f;
};