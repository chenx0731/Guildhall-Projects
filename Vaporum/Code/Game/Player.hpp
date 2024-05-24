#pragma once
#include "Game/Entity.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Math/IntVec2.hpp"

class Unit;

enum class PlayerState
{
	Connected,
	Ready,
	EndTurn,
	WaitingForTurn,
	Selecting,
	UnitSelected,
	UnitMoved,
	UnitMoveConfirmed,
	UnitAttacked
};

class Player : public Entity
{
public:
	Player(Game* owner, Vec3 const& startPos);
	~Player();
	virtual void Update() override;
	virtual void Render() const override;

	void StartTurn();
	void EndTurn();
	void ConfirmEndTurn();

	void Select();
	void Cancel();
	void Move();
	void ConfirmMove();
	void Attack();
	void ConfirmAttack();
	void SetCurUnit(IntVec2 currentCoords);
	void SetTarget(IntVec2 targetCoords);

	bool IsUnitMine(const Unit* unit);
public:
	Camera m_camera;
	float m_cameraSpeed = 2.f;
	float m_cameraSprintSpeed = 20.f;
	float m_cameraDefaultSpeed = 10.f;

	std::vector<Unit*> m_units;

	IntVec2 m_target;
	IntVec2 m_currentUnit;
	IntVec2 m_OGCoord;
	PlayerState m_state = PlayerState::Connected;
};