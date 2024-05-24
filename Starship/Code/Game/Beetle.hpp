#pragma once
#include "Game/Entity.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Core/Vertex_PCU.hpp"

constexpr int NUM_BEETLE_TRIS = 2;
constexpr int NUM_BEETLE_VERTS = 3 * NUM_BEETLE_TRIS;

class Beetle : public Entity
{
public:
	Beetle(Game* owner, Vec2 const& startPos);
	~Beetle();

	virtual void Update() override;
	virtual void Render() const override;
	virtual void Die() override;
	void Damege();
	void Damege(Vec2 position);

private:
	void InitializeLocalVerts();
	float GetOrientationDegree();

private:
	Vertex_PCU m_localVerts[NUM_BEETLE_VERTS];
	Rgba8 m_color = Rgba8(BEETLE_COLOR_R, BEETLE_COLOR_G, BEETLE_COLOR_B);
};