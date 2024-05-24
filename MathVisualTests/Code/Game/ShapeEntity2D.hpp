#pragma once
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Math/OBB2.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/LineSegment2.hpp"
#include "Engine/Math/Capsule2.hpp"
#include "Engine/Math/RaycastResult2D.hpp"
#include <vector>

class ShapeEntity2D
{
public:
	virtual ~ShapeEntity2D();
	virtual void Randomize() = 0;
	virtual void UpdateForCursorPos(Vec2 const& cursorPos) = 0;
	virtual void AddVertsForMe(std::vector<Vertex_PCU>& verts, Rgba8 const& color ) const = 0;
	virtual Vec2 GetNearestPointForMe(Vec2 const& point) const = 0;
	void Render() const;
	void UpdateForRaycast(Vec2 const& start, Vec2 const& end);
	virtual RaycastResult2D IsImpactedByRaycast();
	virtual void SetPositionAndVelocity(Vec2 position, Vec2 velocity);
protected:
	std::vector<Vertex_PCU> m_verts;
	bool m_isCursorIn;
	Vec2 m_cursor;
	Vec2 m_rayStart = Vec2(0.f, 0.f);
	Vec2 m_rayEnd = Vec2(0.f, 0.f);
public:
	RaycastResult2D m_rayCastResult;
	Rgba8 m_color;
	Vec2 m_velocity;
	Vec2 m_acceleration;
	float m_elasticity = 0.9f;
};

class ShapeEntityDisc2D : public ShapeEntity2D
{
public:
	virtual void Randomize() override;
	virtual void UpdateForCursorPos(Vec2 const& cursorPos) override;
	virtual void AddVertsForMe(std::vector<Vertex_PCU>& verts, Rgba8 const& color) const override;
	virtual Vec2 GetNearestPointForMe(Vec2 const& point) const override ;
	virtual RaycastResult2D IsImpactedByRaycast() override;
	virtual void SetPositionAndVelocity(Vec2 position, Vec2 velocity) override;
public:
	Vec2	m_center;
	float	m_radius;
};

class ShapeEntityOBB2D : public ShapeEntity2D
{
public:
	virtual void Randomize() override;
	virtual void UpdateForCursorPos(Vec2 const& cursorPos) override;
	virtual void AddVertsForMe(std::vector<Vertex_PCU>& verts, Rgba8 const& color) const override;
	virtual Vec2 GetNearestPointForMe(Vec2 const& point) const override;
public:
	OBB2	m_box;
};

class ShapeEntityAABB2D : public ShapeEntity2D
{
public:
	virtual void Randomize() override;
	virtual void UpdateForCursorPos(Vec2 const& cursorPos) override;
	virtual void AddVertsForMe(std::vector<Vertex_PCU>& verts, Rgba8 const& color) const override;
	virtual Vec2 GetNearestPointForMe(Vec2 const& point) const override;
	virtual RaycastResult2D IsImpactedByRaycast() override;
protected:
	AABB2	m_box;
};

class ShapeEntityLineSegment2D : public ShapeEntity2D
{
public:
	virtual void Randomize() override;
	virtual void UpdateForCursorPos(Vec2 const& cursorPos) override;
	virtual void AddVertsForMe(std::vector<Vertex_PCU>& verts, Rgba8 const& color) const override;
	virtual Vec2 GetNearestPointForMe(Vec2 const& point) const override;
	virtual RaycastResult2D IsImpactedByRaycast() override;
protected: 
	LineSegment2 m_line;
};

class ShapeEntityInfiniteLine2D : public ShapeEntity2D
{
public:
	virtual void Randomize() override;
	virtual void UpdateForCursorPos(Vec2 const& cursorPos) override;
	virtual void AddVertsForMe(std::vector<Vertex_PCU>& verts, Rgba8 const& color) const override;
	virtual Vec2 GetNearestPointForMe(Vec2 const& point) const override;
protected:
	LineSegment2 m_infiniteLine;
};

class ShapeEntityCapsule2D : public ShapeEntity2D
{
public:
	virtual void Randomize() override;
	virtual void UpdateForCursorPos(Vec2 const& cursorPos) override;
	virtual void AddVertsForMe(std::vector<Vertex_PCU>& verts, Rgba8 const& color) const override;
	virtual Vec2 GetNearestPointForMe(Vec2 const& point) const override;
public:
	Capsule2 m_capsule;
};

class Billiard : public ShapeEntityDisc2D
{
public:
	void SetPositionAndVelocity(Vec2 position, Vec2 velocity);
public:
	Vec2 m_velocity;
	float m_elasticity = 0.9f;
};