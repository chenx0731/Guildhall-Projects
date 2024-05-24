#pragma once
#include "Engine/Core/Vertex_PNCU.hpp"

Vertex_PCUTBN::Vertex_PCUTBN()
{

}

Vertex_PCUTBN::Vertex_PCUTBN(Vec3 const& position, Rgba8 const& tint, Vec2 const& uvTexCoords, Vec3 const& normal)
{
	m_position = position;
	m_color = tint;
	m_uvTexCoords = uvTexCoords;
	m_normal = normal;
}

Vertex_PCUTBN::Vertex_PCUTBN(Vec3 const& position, Rgba8 const& tint, Vec2 const& uvTexCoords, Vec3 const& tangent, Vec3 const& binormal, Vec3 const& normal /*= Vec3()*/)
{
	m_position = position;
	m_color = tint;
	m_uvTexCoords = uvTexCoords;
	m_tangent = tangent;
	m_binormal = binormal;
	m_normal = normal;
}
