#pragma once
#include "Engine/Math/Vec3.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Math/Vec2.hpp"
struct Vertex_PCUTBN
{
public:
	Vertex_PCUTBN();
	explicit Vertex_PCUTBN(Vec3 const& position, Rgba8 const& tint, Vec2 const& uvTexCoords, Vec3 const& normal = Vec3());
	explicit Vertex_PCUTBN(Vec3 const& position, Rgba8 const& tint, Vec2 const& uvTexCoords, Vec3 const& tangent, Vec3 const& binormal, Vec3 const& normal = Vec3());
public:
	Vec3 m_position;
	Rgba8 m_color;
	Vec2 m_uvTexCoords;
	Vec3 m_tangent = Vec3();
	Vec3 m_binormal = Vec3();
	Vec3 m_normal;
};