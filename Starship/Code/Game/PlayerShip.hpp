#pragma once
#include "Game/Entity.hpp"
#include "Game/GameCommon.hpp"

constexpr int NUM_SHIP_TRIS = 6;
constexpr int NUM_SHIP_VERTS = 3 * NUM_SHIP_TRIS;

class PlayerShip : public Entity
{
public:
	PlayerShip(Game* owner, Vec2 const& startPos);
	~PlayerShip();

	virtual void Update() override;
	virtual void Render() const override;
	virtual void Die() override;

	static void const CopyPlayershipVerts(Vertex_PCU* original);

private:
	void InitializeLocalVerts();
	void UpdateFromKeyboard();
	void UpdateFromController();
	void BounceOffWalls();

	void Respawn();

public:
	Vertex_PCU m_localVerts[NUM_SHIP_VERTS];
private:
	Rgba8 m_color = Rgba8(PLAYER_SHIP_COLOR_R, PLAYER_SHIP_COLOR_G, PLAYER_SHIP_COLOR_B);
	bool m_isTurningLeft = false;
	bool m_isTurningRight = false;
	bool m_isThrustingKeyboard = false;
	bool m_isThrustingController = false;
	float m_thrustFractionKeyboard = 0.f;
	float m_thrustFractionController = 0.f;
	float m_hangingTime = 0.f;
	float m_vibrationTime = 0.f;
};