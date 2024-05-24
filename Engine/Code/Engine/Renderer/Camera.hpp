#pragma once
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/EulerAngles.hpp"
#include "Engine/Math/AABB2.hpp"
struct Mat44;
class Camera
{
public:
	enum Mode
	{
		EMODE_ORTHO,
		EMODE_PERSPECTIVE,

		EMODE_NUM
	};
	void SetOrthoView(Vec2 const& bottomLeft, Vec2 const& topRight, float near = 0.f, float far = 1.f);
	void SetSelfOrthoView();
	void SetPerspectiveView(float aspect, float fov, float near, float far);

	Vec2 GetOrthoBottomLeft() const;
	Vec2 GetOrthoTopRight() const;
	void Translate2D(const Vec2& translation2D);

	Mat44 GetOrthoMatrix() const;
	Mat44 GetModelMatrix() const;
	Mat44 GetPerspectiveMatrix() const;
	Mat44 GetProjectionMatrix() const;

	void SetRenderBasis(Vec3 const& iBasis, Vec3 const& jBasis, Vec3 const& kBasis);
	Mat44 GetRenderMatrix() const;

	void SetTransform(const Vec3& position, const EulerAngles& orientation);
	Mat44 GetViewMatrix() const;
public:
	Mode m_mode = EMODE_ORTHO;

	Vec2 m_bottomLeft;
	Vec2 m_topRight;
	float m_orthoNear = 0.f;
	float m_orthoFar = 0.f;

	float m_aspect = 2.f;
	float m_FOV = 0.f;
	float m_perspectiveNear = 0.f;
	float m_perspectiveFar = 0.f;

	Vec3 m_renderIBasis = Vec3(1.f, 0.f, 0.f);
	Vec3 m_renderJBasis = Vec3(0.f, 1.f, 0.f);
	Vec3 m_renderKBasis = Vec3(0.f, 0.f, 1.f);

	AABB2 m_viewport;

	Vec3 m_position;
	EulerAngles m_orientation;
};