#include "Game/Unit.hpp"
#include "Game/UnitDefinition.hpp"
#include "Game/Model.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Mat44.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/Renderer.hpp"

bool Unit::IsPointInside(const Vec3& point) const
{
	Vec3 verts[6];
	Vec3 i = Vec3(1.f, 0.f, 0.f) * m_radius;
	Vec3 j = Vec3(0.5f, 0.866f, 0.f) * m_radius;
	Vec3 k = Vec3(-0.5f, 0.866f, 0.f) * m_radius;
	verts[0] = m_position + i;
	verts[1] = m_position + j;
	verts[2] = m_position + k;
	verts[3] = m_position - i;
	verts[4] = m_position - j;
	verts[5] = m_position - k;
	bool result = true;
	for (int index = 0; index < 6; index++) {
		Vec3 edge = verts[(index + 1) % 6] - verts[index];
		Vec3 T = point - verts[index];
		Vec3 N = CrossProduct3D(edge, T);
		if (N.z <= 0.f) {
			result = false;
			break;
		}
	}
	return result;
}

void Unit::Render() const
{
	if (m_state == UnitState::Dead)
		return;
	Mat44 mat = m_orientation.GetMatrix_XFwd_YLeft_ZUp();
	mat.AppendTranslation3D(m_position);
	if (m_state == UnitState::Finished)
		g_theRenderer->SetModelConstants(mat, Rgba8::GREY);
	else g_theRenderer->SetModelConstants(mat, m_color);
	m_unitDef->m_model->Render();

}

void Unit::RenderSelected() const
{
	std::vector<Vertex_PCU> unit;
	Vec3 pos = m_position;
	pos.z = 0.02f;
	if (m_state == UnitState::Selected)
		AddVertsForHexgon3DFrame(unit, 0.5f, pos, 0.03f, Rgba8::BLUE);
	if (m_state == UnitState::Moved)
		AddVertsForHexgon3DFrame(unit, 0.5f, pos, 0.05f, Rgba8::BLUE);
	if (m_state == UnitState::Targeted)
		AddVertsForHexgon3DFrame(unit, 0.5f, pos, 0.03f, Rgba8::RED);
	if (m_state == UnitState::Damaged)
		AddVertsForHexgon3DFrame(unit, 0.5f, pos, 0.05f, Rgba8::RED);
	g_theRenderer->DrawVertexArray((int)unit.size(), unit.data());
	//AddVertsForHexgon3DFrame(unit, )
}

IntVec2 Unit::GetUnitCoords() const
{
	return m_coords; 
}

void Unit::TakeDamage(float damage)
{
	m_currentHealth -= damage * 2 / m_unitDef->m_groundDefenese;
	if (m_currentHealth < 1.f) {
		m_state = UnitState::Dead;
	}
}

float Unit::GetHexCrabDist(const IntVec2& coords) const
{
	int distance = (Abs(m_coords.x - coords.x) +
					Abs(m_coords.x + m_coords.y - coords.x - coords.y) + 
					Abs(m_coords.y - coords.y) ) / 2;
	return float(distance);
}

bool Unit::IsCoordsInAttackRange(const IntVec2& coords) const
{
	float distance = GetHexCrabDist(coords);
	if (distance <= m_unitDef->m_groundAttackRange_Max 
		&& distance >= m_unitDef->m_groundAttackRange_Min) {
		return true;
	}
	return false;
}

