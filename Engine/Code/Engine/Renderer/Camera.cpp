#pragma once
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Math/Mat44.hpp"

void Camera::SetOrthoView(Vec2 const& bottomLeft, Vec2 const& topRight, float near, float far)
{
	m_mode = EMODE_ORTHO;
	m_bottomLeft = bottomLeft;
	m_topRight = topRight;
	m_orthoNear = near;
	m_orthoFar = far;
}

void Camera::SetSelfOrthoView()
{
	SetOrthoView(m_bottomLeft, m_topRight);
}

void Camera::SetPerspectiveView(float aspect, float fov, float near, float far)
{
	m_mode = EMODE_PERSPECTIVE;
	m_aspect = aspect;
	m_FOV = fov;
	m_perspectiveNear = near;
	m_perspectiveFar = far;
}

Vec2 Camera::GetOrthoBottomLeft() const
{
	return m_bottomLeft;
}

Vec2 Camera::GetOrthoTopRight() const
{
	return m_topRight;
}


void Camera::Translate2D(const Vec2& translation2D)
{
	m_bottomLeft += translation2D;
	m_topRight += translation2D;
}

Mat44 Camera::GetOrthoMatrix() const
{
	return Mat44::CreateOrthoProjection(m_bottomLeft.x, m_topRight.x, m_bottomLeft.y, m_topRight.y, m_orthoNear, m_orthoFar);
}

Mat44 Camera::GetModelMatrix() const
{
	Mat44 mat = m_orientation.GetMatrix_XFwd_YLeft_ZUp();
	mat.SetTranslation3D(m_position);
	return mat;
}

Mat44 Camera::GetPerspectiveMatrix() const
{
	return Mat44::CreatePerspectiveProjection(m_FOV, m_aspect, m_perspectiveNear, m_perspectiveFar);
}

Mat44 Camera::GetProjectionMatrix() const
{
	Mat44 mat;
	if (m_mode == EMODE_ORTHO)
		mat = GetOrthoMatrix();
	if (m_mode == EMODE_PERSPECTIVE)
		mat = GetPerspectiveMatrix();
	mat.Append(GetRenderMatrix());
	return mat;
}

void Camera::SetRenderBasis(Vec3 const& iBasis, Vec3 const& jBasis, Vec3 const& kBasis)
{
	m_renderIBasis = iBasis;
	m_renderJBasis = jBasis;
	m_renderKBasis = kBasis;
}

Mat44 Camera::GetRenderMatrix() const
{
	Mat44 mat = Mat44(m_renderIBasis, m_renderJBasis, m_renderKBasis, Vec3(0.f, 0.f, 0.f));
	return mat;
}

void Camera::SetTransform(const Vec3& position, const EulerAngles& orientation)
{
	m_position = position;
	m_orientation = orientation;
}

Mat44 Camera::GetViewMatrix() const
{
	Mat44 mat = m_orientation.GetMatrix_XFwd_YLeft_ZUp();
	mat.SetTranslation3D(m_position);
	return mat.GetOrthonormalInverse();;
}
