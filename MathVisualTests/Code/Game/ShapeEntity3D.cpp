#include "ShapeEntity3D.hpp"
#include "Game/GameCommon.hpp"
#include "Game/App.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Core/VertexUtils.hpp"

ShapeEntity3D::~ShapeEntity3D()
{
}

void ShapeEntity3D::Render() const
{
	g_theRenderer->SetModelConstants(GetModelMatrix(), m_color);
	g_theRenderer->SetBlendMode(BlendMode::ALPHA);
	//g_theRenderer->SetRasterizerMode(RasterizerMode::WIREFRAME_CULL_BACK);
	g_theRenderer->BindTexture(m_texture);
	//TransformVertexArrayXY3D()
	g_theRenderer->DrawVertexArray((int)m_verts.size(), m_verts.data());
	g_theRenderer->BindTexture(nullptr);
}

void ShapeEntity3D::UpdateForRaycast(Vec2 const& start, Vec2 const& end)
{
	m_rayStart = start;
	m_rayEnd = end;
}

Mat44 ShapeEntity3D::GetModelMatrix() const
{
	Mat44 mat = m_orientation.GetMatrix_XFwd_YLeft_ZUp();
	mat.SetTranslation3D(m_position);
	return mat;
}

RaycastResult2D ShapeEntity3D::IsImpactedByRaycast()
{
	return RaycastResult2D();
}
