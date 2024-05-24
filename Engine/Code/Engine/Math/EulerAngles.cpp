#include "Engine/Math/EulerAngles.hpp"
#include "Engine/Math/Mat44.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"

EulerAngles::EulerAngles(float yawDegrees, float pitchDegrees, float rollDegrees)
{
	m_yawDegrees = yawDegrees;
	m_pitchDegrees = pitchDegrees;
	m_rollDegrees = rollDegrees;
}

void EulerAngles::GetVectors_XFwd_YLeft_ZUp(Vec3& out_forwardIBasis, Vec3& out_leftJBasis, Vec3& out_upKBasis) const
{
	out_forwardIBasis.x = CosDegrees(m_yawDegrees) * CosDegrees(m_pitchDegrees);
	out_forwardIBasis.y = SinDegrees(m_yawDegrees) * CosDegrees(m_pitchDegrees);
	out_forwardIBasis.z = -SinDegrees(m_pitchDegrees);                           

	out_leftJBasis.x = -SinDegrees(m_yawDegrees) * CosDegrees(m_rollDegrees) + CosDegrees(m_yawDegrees) * SinDegrees(m_pitchDegrees) * SinDegrees(m_rollDegrees);
	out_leftJBasis.y = CosDegrees(m_yawDegrees) * CosDegrees(m_rollDegrees) + SinDegrees(m_yawDegrees) * SinDegrees(m_pitchDegrees) * SinDegrees(m_rollDegrees);
	out_leftJBasis.z = SinDegrees(m_rollDegrees) * CosDegrees(m_pitchDegrees);

	out_upKBasis.x = SinDegrees(m_yawDegrees) * SinDegrees(m_rollDegrees) + CosDegrees(m_yawDegrees) * SinDegrees(m_pitchDegrees) * CosDegrees(m_rollDegrees);
	out_upKBasis.y = CosDegrees(m_rollDegrees) * SinDegrees(m_yawDegrees) * SinDegrees(m_pitchDegrees) - SinDegrees(m_rollDegrees) * CosDegrees(m_yawDegrees);
	out_upKBasis.z = CosDegrees(m_rollDegrees) * CosDegrees(m_pitchDegrees);
}

Mat44 EulerAngles::GetMatrix_XFwd_YLeft_ZUp() const
{
	Mat44 output = Mat44();
	
	output.AppendZRotation(m_yawDegrees);
	output.AppendYRotation(m_pitchDegrees);
	output.AppendXRotation(m_rollDegrees);

	return output;
}

Mat44 EulerAngles::GetMatrix_YFwd_ZLeft_XUp() const
{
	Mat44 output = Mat44();
	output.AppendXRotation(m_rollDegrees);
	output.AppendZRotation(m_yawDegrees);
	output.AppendYRotation(m_pitchDegrees);	
	return output;
}

Vec3 EulerAngles::GetOrientationVector() const
{
	float x = -CosDegrees(m_yawDegrees) * SinDegrees(m_pitchDegrees) * SinDegrees(m_rollDegrees) - SinDegrees(m_yawDegrees) * CosDegrees(m_rollDegrees);
	float y = -SinDegrees(m_yawDegrees) * SinDegrees(m_pitchDegrees) * SinDegrees(m_rollDegrees) + CosDegrees(m_yawDegrees) * CosDegrees(m_rollDegrees);
	float z = CosDegrees(m_pitchDegrees) * SinDegrees(m_rollDegrees);

	return Vec3(x, y, z);
}

Vec3 EulerAngles::GetForwardVector() const
{
	float x = CosDegrees(m_yawDegrees) * CosDegrees(m_pitchDegrees);
	float y = SinDegrees(m_yawDegrees) * CosDegrees(m_pitchDegrees);
	float z = -SinDegrees(m_pitchDegrees);

	return Vec3(x, y, z);
}

void EulerAngles::SetFromText(char const* text, char const* split)
{
	Strings strings = SplitStringOnDelimiter(text, split);
	if (strings.size() < 3)
		ERROR_AND_DIE("Lack of variables when set Vec3");
	m_yawDegrees = static_cast<float>(atof(strings[0].c_str()));
	m_pitchDegrees = static_cast<float>(atof(strings[1].c_str()));
	m_rollDegrees = static_cast<float>(atof(strings[2].c_str()));
}