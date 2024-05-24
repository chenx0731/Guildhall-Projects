#pragma once
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Math/EulerAngles.hpp"
#include "Engine/Math/RaycastResult2D.hpp"
#include <vector>

class Texture;

class ShapeEntity3D
{
public:
	virtual ~ShapeEntity3D();
	void Render() const;
	void UpdateForRaycast(Vec2 const& start, Vec2 const& end);
	Mat44 GetModelMatrix() const;
	virtual RaycastResult2D IsImpactedByRaycast();
protected:
	//std::vector<Vertex_PCU> m_verts;
	bool m_isCursorIn;
	Vec2 m_cursor;
	Vec2 m_rayStart = Vec2(0.f, 0.f);
	Vec2 m_rayEnd = Vec2(0.f, 0.f);
public:
	std::vector<Vertex_PCU> m_verts;
	RaycastResult2D m_rayCastResult;
	Rgba8 m_color = Rgba8::WHITE;
	Vec3 m_position;
	EulerAngles m_orientation;
	Texture* m_texture = nullptr;
};

