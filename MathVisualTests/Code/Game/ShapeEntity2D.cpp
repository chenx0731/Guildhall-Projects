#include "ShapeEntity2D.hpp"
#include "Game/GameCommon.hpp"
#include "Game/App.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Core/VertexUtils.hpp"

ShapeEntity2D::~ShapeEntity2D()
{
}

void ShapeEntity2D::Render() const
{
	std::vector<Vertex_PCU> tempVerts;
	tempVerts.reserve(200);
	switch (g_theApp->m_gameStatus)
	{
	case GAME_STATUS_NULL:
		break;
	case GAME_STATUS_NEAREST_POINT:
	{
		AddVertsForMe(tempVerts, m_isCursorIn ? Rgba8(100, 150, 255, 200) : Rgba8(50, 80, 150, 200));
		Vec2 nearestPoint = GetNearestPointForMe(m_cursor);
		g_theRenderer->DrawVertexArray(static_cast<int> (tempVerts.size()), tempVerts.data());
		DrawCircle(nearestPoint, 0.8f, Rgba8(255, 100, 0));
		DebugDrawLine(nearestPoint, m_cursor, 0.1f, Rgba8(255, 255, 255, 100));
		break;
	}
	case GAME_STATUS_RAYCAST_DISC_2D:
	{
		AddVertsForMe(tempVerts, m_rayCastResult.m_didImpact ? Rgba8(100, 150, 255, 200) : Rgba8(50, 80, 150, 200));
		g_theRenderer->DrawVertexArray(static_cast<int> (tempVerts.size()), tempVerts.data());
		break;
	}
	case GAME_STATUS_BILLIARDS_2D:
	{
		Rgba8 color = m_color;
		if (m_rayCastResult.m_didImpact) {
			color.a += 50;
		}
		AddVertsForMe(tempVerts, color);
		g_theRenderer->DrawVertexArray(static_cast<int> (tempVerts.size()), tempVerts.data());
		break;
	}
	case GAME_STATUS_RAYCAST_LINE_2D:
	{
		AddVertsForMe(tempVerts, m_rayCastResult.m_didImpact ? Rgba8(100, 255, 150, 200) : Rgba8(50, 150, 80, 200));
		g_theRenderer->DrawVertexArray(static_cast<int> (tempVerts.size()), tempVerts.data());
		break;
	}
	case GAME_STATUS_RAYCAST_AABB_2D:
	{
		AddVertsForMe(tempVerts, m_rayCastResult.m_didImpact ? Rgba8(255, 150, 100, 200) : Rgba8(150, 80, 50, 200));
		g_theRenderer->DrawVertexArray(static_cast<int> (tempVerts.size()), tempVerts.data());
		break;
	}
	case GAME_STATUS_RAYCAST_CONVEX:
	{
		AddVertsForMe(tempVerts, m_rayCastResult.m_didImpact ? Rgba8(255, 150, 100, 200) : Rgba8(150, 80, 50, 200));
		g_theRenderer->DrawVertexArray(static_cast<int> (tempVerts.size()), tempVerts.data());
		break;
	}
	case GAME_STATUS_PACHINKO_2D:
	{
		/*Rgba8 color = m_color;
		if (m_rayCastResult.m_didImpact) {
			color.a += 50;
		}*/
		AddVertsForMe(tempVerts, m_color);
		g_theRenderer->DrawVertexArray(static_cast<int> (tempVerts.size()), tempVerts.data());
		break;
	}
	case GAME_STATUS_NUM:
		break;
	}

	/*AddVertsForMe(tempVerts, m_isCursorIn ? Rgba8(100, 150, 255, 200) : Rgba8(50, 80, 150, 200));
	Vec2 nearestPoint = GetNearestPointForMe(m_cursor);
	g_theRenderer->DrawVertexArray(static_cast<int> (tempVerts.size()), tempVerts.data());
	DrawCircle(nearestPoint, 0.8f, Rgba8(255, 100, 0));
	DebugDrawLine(nearestPoint, m_cursor, 0.1f, Rgba8(255, 255, 255, 100));*/
}

void ShapeEntity2D::UpdateForRaycast(Vec2 const& start, Vec2 const& end)
{
	m_rayStart = start;
	m_rayEnd = end;
}

RaycastResult2D ShapeEntity2D::IsImpactedByRaycast()
{
	return RaycastResult2D();
}

void ShapeEntity2D::SetPositionAndVelocity(Vec2 position, Vec2 velocity)
{
}

RaycastResult2D ShapeEntityDisc2D::IsImpactedByRaycast()
{
	Vec2 fwdNormal = (m_rayEnd - m_rayStart).GetNormalized();
	float length = (m_rayEnd - m_rayStart).GetLength();
	m_rayCastResult = RaycastVsDisc2D(m_rayStart, fwdNormal, length, m_center, m_radius);
	return m_rayCastResult;
}

void ShapeEntityDisc2D::SetPositionAndVelocity(Vec2 position, Vec2 velocity)
{
	m_center = position;
	m_velocity = velocity;
}


void ShapeEntityDisc2D::Randomize()
{
	float x = g_rng->RollRandomFloatInRange(20.f, 180.f);
	float y = g_rng->RollRandomFloatInRange(20.f, 80.f);
	m_center = Vec2(x, y);
	m_radius = g_rng->RollRandomFloatInRange(3.f, 7.f);
}

void ShapeEntityDisc2D::UpdateForCursorPos(Vec2 const& cursorPos)
{
	m_isCursorIn = IsPointInsideDisc2D(cursorPos, m_center, m_radius);
	m_cursor = cursorPos;
}

void ShapeEntityDisc2D::AddVertsForMe(std::vector<Vertex_PCU>& verts, Rgba8 const& color) const
{
	AddVertsForDisc2D(verts, m_center, m_radius, color);
}

Vec2 ShapeEntityDisc2D::GetNearestPointForMe(Vec2 const& point) const
{
	return GetNearestPointOnDisc2D(point, m_center, m_radius);
}


void ShapeEntityOBB2D::Randomize()
{
	float x1 = g_rng->RollRandomFloatInRange(20.f, WORLD_SIZE_X - 20.f);
	float y1 = g_rng->RollRandomFloatInRange(10.f, WORLD_SIZE_Y - 10.f);
	float degree = g_rng->RollRandomFloatInRange(0.f, 360.f);
	float width = g_rng->RollRandomFloatInRange(1.f, 5.f);
	float height = g_rng->RollRandomFloatInRange(1.f, 5.f);
	m_box = OBB2(Vec2(x1, y1), Vec2::MakeFromPolarDegrees(degree), Vec2(width, height));
}

void ShapeEntityOBB2D::UpdateForCursorPos(Vec2 const& cursorPos)
{
	m_isCursorIn = IsPointInsideOBB2D(cursorPos, m_box);
	m_cursor = cursorPos;
}

void ShapeEntityOBB2D::AddVertsForMe(std::vector<Vertex_PCU>& verts, Rgba8 const& color) const
{
	AddVertsForOBB2D(verts, m_box, color);
}

Vec2 ShapeEntityOBB2D::GetNearestPointForMe(Vec2 const& point) const
{
	return GetNearestPointOnOBB2D(point, m_box);
}


void ShapeEntityAABB2D::Randomize()
{
	float x1 = g_rng->RollRandomFloatInRange(20.f, WORLD_SIZE_X - 20.f);
	float x2 = g_rng->RollRandomFloatInRange(x1 + 2.f, x1 + 15.f);
	float y1 = g_rng->RollRandomFloatInRange(10.f, WORLD_SIZE_Y - 10.f);
	float y2 = g_rng->RollRandomFloatInRange(y1 + 2.f, y1 + 15.f);
	m_box.m_mins = Vec2(x1, y1);
	m_box.m_maxs = Vec2(x2, y2);
}

void ShapeEntityAABB2D::UpdateForCursorPos(Vec2 const& cursorPos)
{
	m_isCursorIn = IsPointInsideAABB2D(cursorPos, m_box);
	m_cursor = cursorPos;
}

RaycastResult2D ShapeEntityAABB2D::IsImpactedByRaycast()
{
	Vec2 fwdNormal = (m_rayEnd - m_rayStart).GetNormalized();
	float length = (m_rayEnd - m_rayStart).GetLength();
	m_rayCastResult = RaycastVsAABB2D(m_rayStart, fwdNormal, length, m_box);
	return m_rayCastResult;
}


void ShapeEntityAABB2D::AddVertsForMe(std::vector<Vertex_PCU>& verts, Rgba8 const& color) const
{
	AddVertsForAABB2D(verts, m_box, color);
}

Vec2 ShapeEntityAABB2D::GetNearestPointForMe(Vec2 const& point) const
{
	return GetNearestPointOnAABB2D(point, m_box);
}


void ShapeEntityLineSegment2D::Randomize()
{
	float x1 = g_rng->RollRandomFloatInRange(5.f, WORLD_SIZE_X - 5.f);
	float degree = g_rng->RollRandomFloatInRange(0.f, 360.f);
	Vec2 direction = Vec2::MakeFromPolarDegrees(degree);
	float length = g_rng->RollRandomFloatInRange(2.f, 20.f);
	float y1 = g_rng->RollRandomFloatInRange(5.f, WORLD_SIZE_Y - 5.f);
	m_line.m_start = Vec2(x1, y1);
	m_line.m_end = m_line.m_start + direction * length;
}

void ShapeEntityLineSegment2D::UpdateForCursorPos(Vec2 const& cursorPos)
{
	m_cursor = cursorPos;
}

void ShapeEntityLineSegment2D::AddVertsForMe(std::vector<Vertex_PCU>& verts, Rgba8 const& color) const
{
	switch (g_theApp->m_gameStatus)
	{
	case GAME_STATUS_NULL:
		break;
	case GAME_STATUS_NEAREST_POINT:
	{
		AddVertsForLineSegment2D(verts, m_line, 2.f, color);
		break;
	}

	case GAME_STATUS_RAYCAST_LINE_2D:
	{
		AddVertsForLineSegment2D(verts, m_line, 0.3f, color);
		break;
	}
	case GAME_STATUS_RAYCAST_CONVEX:
	{
		AddVertsForLineSegment2D(verts, m_line, 0.3f, color);
		break;
	}

	}
	//AddVertsForLineSegment2D(verts, m_line, 2.f, color);
}

Vec2 ShapeEntityLineSegment2D::GetNearestPointForMe(Vec2 const& point) const
{
	return GetNearestPointOnLineSegment2D(point, m_line);
}

RaycastResult2D ShapeEntityLineSegment2D::IsImpactedByRaycast()
{
	Vec2 fwdNormal = (m_rayEnd - m_rayStart).GetNormalized();
	float length = (m_rayEnd - m_rayStart).GetLength();
	m_rayCastResult = RaycastVsLine2D(m_rayStart, fwdNormal, length, m_line);
	//RaycastVsInfiniteLine2D
	return m_rayCastResult;
}

void ShapeEntityCapsule2D::Randomize()
{
	float x1 = g_rng->RollRandomFloatInRange(5.f, WORLD_SIZE_X - 5.f);
	float degree = g_rng->RollRandomFloatInRange(0.f, 360.f);
	Vec2 direction = Vec2::MakeFromPolarDegrees(degree);
	float length = g_rng->RollRandomFloatInRange(2.f, 10.f);
	float y1 = g_rng->RollRandomFloatInRange(5.f, WORLD_SIZE_Y - 5.f);
	Vec2 start = Vec2(x1, y1);
	Vec2 end = start + direction * length;

	LineSegment2 line = LineSegment2(start, end);
	float radius = g_rng->RollRandomFloatInRange(2.f, 10.f);
	m_capsule = Capsule2(line, radius);
}

void ShapeEntityCapsule2D::UpdateForCursorPos(Vec2 const& cursorPos)
{
	m_isCursorIn = IsPointInsideCapsule2D(cursorPos, m_capsule);
	m_cursor = cursorPos;
}

void ShapeEntityCapsule2D::AddVertsForMe(std::vector<Vertex_PCU>& verts, Rgba8 const& color) const
{
	AddVertsForCapsule2D(verts, m_capsule, color);
}

Vec2 ShapeEntityCapsule2D::GetNearestPointForMe(Vec2 const& point) const
{
	return GetNearestPointOnCapsule2D(point, m_capsule);
}


void ShapeEntityInfiniteLine2D::Randomize()
{
	float x1 = g_rng->RollRandomFloatInRange(0.f, WORLD_SIZE_X);
	float x2 = g_rng->RollRandomFloatInRange(0.f, WORLD_SIZE_X);
	float y1 = g_rng->RollRandomFloatInRange(0.f, WORLD_SIZE_Y);
	float y2 = g_rng->RollRandomFloatInRange(0.f, WORLD_SIZE_Y);
	m_infiniteLine.m_start = Vec2(x1, y1);
	m_infiniteLine.m_end = Vec2(x2, y2);
}

void ShapeEntityInfiniteLine2D::UpdateForCursorPos(Vec2 const& cursorPos)
{
	m_cursor = cursorPos;
}

void ShapeEntityInfiniteLine2D::AddVertsForMe(std::vector<Vertex_PCU>& verts, Rgba8 const& color) const
{
	Vec2 line = m_infiniteLine.m_end - m_infiniteLine.m_start;
	line.SetLength(400.f);
	AddVertsForLineSegment2D(verts, -line + m_infiniteLine.m_end, line + m_infiniteLine.m_start, 2.f, color);
}

Vec2 ShapeEntityInfiniteLine2D::GetNearestPointForMe(Vec2 const& point) const
{
	return GetNearestPointOnInfiniteLine2D(point, m_infiniteLine);
}

void Billiard::SetPositionAndVelocity(Vec2 position, Vec2 velocity)
{
	m_center = position;
	m_velocity = velocity;
}
