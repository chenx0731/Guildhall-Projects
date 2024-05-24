#pragma once
#include "Game/Entity.hpp"
#include "Game/GameCommon.hpp"
//#include "Engine/Core/Vertex_PCU.hpp"

constexpr int NUM_WASP_SIDES = 16;
constexpr int NUM_WASP_TRIS = NUM_WASP_SIDES;
constexpr int NUM_WASP_VERTS = 3 * NUM_WASP_TRIS;

class Wasp : public Entity
{
public:
	Wasp(Game* owner, Vec2 const& startPos);
	~Wasp();

	virtual void Update() override;
	virtual void Render() const override;
	virtual void Die() override;

	void Damege(Vec2 position);
	void Damege();

private:
	void InitializeLocalVerts();
	float GetOrientationDegree();
private:
	Vertex_PCU m_localVerts[NUM_WASP_VERTS];
	int m_renderIndex = 0;
	Rgba8 m_color = Rgba8(WASP_COLOR_R, WASP_COLOR_G, WASP_COLOR_B);
};