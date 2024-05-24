#pragma once

struct Vec3;
struct Mat44;

struct EulerAngles
{
public:
	EulerAngles() = default;
	EulerAngles(float yawDegrees, float pitchDegrees, float rollDegrees);
	void GetVectors_XFwd_YLeft_ZUp(Vec3& out_forwardIBasis, Vec3& out_leftJBasis, Vec3& out_upKBasis) const;
	Mat44 GetMatrix_XFwd_YLeft_ZUp() const;
	Mat44 GetMatrix_YFwd_ZLeft_XUp() const;
	Vec3 GetOrientationVector() const;
	Vec3 GetForwardVector() const;

	void SetFromText(char const* text, char const* split);

public:
	float m_yawDegrees		= 0.f;
	float m_pitchDegrees	= 0.f;
	float m_rollDegrees		= 0.f;
};