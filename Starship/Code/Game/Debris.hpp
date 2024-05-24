#pragma once
#include "Game/Entity.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Core/Vertex_PCU.hpp"

constexpr int NUM_DEBRIS_MAX_TRI = 8;
constexpr int NUM_DEBRIS_MAX_VERTS = NUM_DEBRIS_MAX_TRI * 3;

class Debris : public Entity
{
public:
	Debris(Game* owner, Vec2 const& startPos, float maxRadius, float minRadius, Rgba8 color);
	~Debris();

	virtual void Update() override;
	virtual void Render() const override;
	virtual void Die() override;

private:
	void InitializeLocalVerts(float maxRadius, float minRadius, Rgba8 color);

private:
	Vertex_PCU m_localVerts[NUM_DEBRIS_MAX_VERTS];
	int m_vertsNum = 0;
	int m_sideNum = 0;
	float m_lifeTime = 0.f;
};