#pragma once
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Math/AABB3.hpp"
#include "Engine/Math/EulerAngles.hpp"
#include <string>

class UnitDefinition;
enum class UnitState {
	Ready,
	Selected,
	Moved,
	Finished,
	Targeted,
	Damaged,
	Dead,
};

class Unit
{
public:
	UnitDefinition const*	m_unitDef = nullptr;
	Vec3					m_position;
	float					m_radius = 0.577f;
	Rgba8					m_color;
	EulerAngles				m_orientation;
	float					m_currentHealth = 0.f;
	UnitState				m_state = UnitState::Ready;
	IntVec2					m_coords;
	//IntVec2					m_tileCoords;
public: 
	bool IsPointInside(const Vec3& point) const;
	void Render() const;
	void RenderSelected() const;
	IntVec2 GetUnitCoords() const;
	void TakeDamage(float damage);
	float GetHexCrabDist(const IntVec2& coords) const;
	bool IsCoordsInAttackRange(const IntVec2& coords) const;

};
