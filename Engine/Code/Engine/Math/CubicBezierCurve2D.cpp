#include "Engine/Math/CubicBezierCurve2D.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Math/LineSegment2.hpp"

CubicBezierCurve2D::CubicBezierCurve2D()
{

}

CubicBezierCurve2D::CubicBezierCurve2D(Vec2 startPos, Vec2 guidePos1, Vec2 guidePos2, Vec2 endPos)
{
	m_startPos = startPos;
	m_guidePos1 = guidePos1;
	m_guidePos2 = guidePos2;
	m_endPos = endPos;

	m_velocityA = 3.f * (guidePos1 - startPos);
	m_velocityD = 3.f * (endPos - guidePos1);
}

CubicBezierCurve2D::CubicBezierCurve2D(CubicBezierCurve2D const& fromHermite)
{
	//m_startPos = fromHermite.m_startPos;
	//m_guidePos1 = fromHermite.m_guidePos1;
	//m_guidePos2 = fromHermite.m_guidePos2;
	//m_endPos = fromHermite.m_endPos;
	m_startPos = fromHermite.m_startPos;
	m_endPos = fromHermite.m_endPos;
	m_velocityA = fromHermite.m_velocityA;
	m_velocityD = fromHermite.m_velocityD;
	RecalculateCurve();
}

Vec2 CubicBezierCurve2D::EvaluateAtParametric(float parametricZeroToOne) const
{
	float x = ComputeCubicBezier1D(m_startPos.x, m_guidePos1.x, m_guidePos2.x, m_endPos.x, parametricZeroToOne);
	float y = ComputeCubicBezier1D(m_startPos.y, m_guidePos1.y, m_guidePos2.y, m_endPos.y, parametricZeroToOne);
	return Vec2(x, y);
}

float CubicBezierCurve2D::GetApproximateLength(int numSubdivision) const
{
	float deltaT = 1.f / (float)numSubdivision;
	float endT = deltaT;
	float length = 0.f;
	Vec2 start = m_startPos;
	for (int divIndex = 0; divIndex < numSubdivision; divIndex++) {
		Vec2 end = EvaluateAtParametric(endT);
		length += (end - start).GetLength();
		endT += deltaT;
		start = end;
	}
	return length;
}

Vec2 CubicBezierCurve2D::EvaluateAtApproximateDistance(float distanceAlongCurve, int numSubdivisions) const
{
	float deltaT = 1.f / (float)numSubdivisions;
	float endT = deltaT;
	float length = 0.f;
	Vec2 start = m_startPos;
	for (int divIndex = 0; divIndex < numSubdivisions; divIndex++) {
		Vec2 end = EvaluateAtParametric(endT);
		float deltaDist = (end - start).GetLength();
		if (distanceAlongCurve >= length && distanceAlongCurve <= length + deltaDist) {
			float t = (distanceAlongCurve - length) / deltaDist;
			float x = Interpolate(start.x, end.x, t);
			float y = Interpolate(start.y, end.y, t);
			return Vec2(x, y);
		}
		length += (end - start).GetLength();
		endT += deltaT;
		start = end;
	}
	return m_endPos;
}

Vec2 CubicBezierCurve2D::EvaluateVelAtParametric(float parametricZeroToOne, bool isPositive) const
{
	float delta = 1.f / 64.f;
	if (parametricZeroToOne == 1.f)
		parametricZeroToOne = 1.f - delta;
	Vec2 start = EvaluateAtParametric(parametricZeroToOne);
	float endTime = parametricZeroToOne + delta;
	GetClamped(endTime, 0.f, 1.f);
	Vec2 end = EvaluateAtParametric(endTime);
	Vec2 vel = end - start;
	vel = vel.GetNormalized();
	if (!isPositive)
		vel = -vel;
	return vel;
}

void CubicBezierCurve2D::SetStartVelocity(Vec2 vel)
{
	m_velocityA = vel;
	RecalculateCurve();
}

void CubicBezierCurve2D::SetEndVelocity(Vec2 vel)
{
	RecalculateCurve();
}

void CubicBezierCurve2D::RecalculateCurve()
{
	m_guidePos1 = m_velocityA / 3.f + m_startPos;
	m_guidePos2 = m_endPos - m_velocityD / 3.f;
}

void CubicBezierCurve2D::AddVertsForWholeCurve(std::vector<Vertex_PCU>& verts, float thickness, Rgba8 color, int numSubdivisions) const
{
	//float sumLength = GetApproximateLength(numSubdivisions);
	float deltaLength = 1.f / (float)numSubdivisions;
	Vec2 start = m_startPos;
	Vec2 end = m_startPos;
	float progress = 0.f;
	for (int segIndex = 0; segIndex < numSubdivisions; segIndex++) {
		start = end;
		progress += deltaLength;
		end = EvaluateAtParametric(progress);
		AddVertsForLineSegment2D(verts, LineSegment2(start, end), thickness, color);
	}
}

void CubicBezierCurve2D::AddVertsForCurveSeg(std::vector<Vertex_PCU>& verts, float parametricZeroToOne, float thickness, Rgba8 color, int numSubdivisions) const
{
	float deltaT = 1.f / (float)numSubdivisions;
	int segmentNum = RoundDownToInt(parametricZeroToOne / deltaT);
	Vec2 start = m_startPos;
	Vec2 end = m_startPos;
	float progress = 0.f;
	for (int segIndex = 0; segIndex < segmentNum; segIndex++) {
		start = end;
		progress += deltaT;
		end = EvaluateAtParametric(progress);
		AddVertsForLineSegment2D(verts, LineSegment2(start, end), thickness, color);
	}
	Vec2 finalEnd = EvaluateAtParametric(parametricZeroToOne);
	AddVertsForLineSegment2D(verts, LineSegment2(end, finalEnd), thickness, color);
}

CubicHermiteSpline::CubicHermiteSpline()
{
}

CubicHermiteSpline::CubicHermiteSpline(const std::vector<Vec2>& points)
{
	//for (int pointIndex = 0; pointIndex < points.size() - 1; pointIndex++) {
	//	CubicBezierCurve2D temp = CubicBezierCurve2D(points[pointIndex], Vec2(), Vec2(), points[pointIndex + 1]);
	//	if (pointIndex != 0) {
	//		temp.m_velocityA = 0.5f * (points[pointIndex + 1] - points[pointIndex - 1]);
	//	}
	//	if (pointIndex != points.size() - 2) {
	//		temp.m_velocityD = 0.5f * (points[pointIndex + 2] - points[pointIndex]);
	//	}
	//	CubicBezierCurve2D curve = CubicBezierCurve2D(temp);
	//	m_curves.push_back(curve);
	//}
	m_points = points;
	for (int pointIndex = 0; pointIndex < points.size(); pointIndex++) {
		if (pointIndex == 0 || pointIndex == points.size() - 1)
			m_vels.push_back(Vec2());
		else
			m_vels.push_back((points[pointIndex + 1] - points[pointIndex - 1]) * 0.5f);
	}
	for (int pointIndex = 0; pointIndex < points.size() - 1; pointIndex++) {
		CubicBezierCurve2D temp = CubicBezierCurve2D(points[pointIndex], Vec2(), Vec2(), points[pointIndex + 1]);
		temp.m_velocityA = m_vels[pointIndex];
		temp.m_velocityD = m_vels[pointIndex + 1];
		CubicBezierCurve2D curve = CubicBezierCurve2D(temp);
		m_curves.push_back(curve);
	}
}

void CubicHermiteSpline::AddVertsForSpline(std::vector<Vertex_PCU>& verts, float thickness, Rgba8 color, int numSubdivisions) const
{
	for (int curveIndex = 0; curveIndex < m_curves.size(); curveIndex++) {
		m_curves[curveIndex].AddVertsForWholeCurve(verts, thickness, color, numSubdivisions);
	}
}


Vec2 CubicHermiteSpline::EvaluateAtParametric(float parametricZeroToOne) const
{
	float deltaT = 1.f / (float)m_curves.size();
	float startT = 0.f;
	float endT = 0.f;
	for (int curveIndex = 0; curveIndex < (int)m_curves.size(); curveIndex++) {
		startT = endT;
		endT += deltaT;
		if (parametricZeroToOne >= startT && parametricZeroToOne <= endT) {
			float rangeMap = RangeMap(parametricZeroToOne, startT, endT, 0.f, 1.f);
			return m_curves[curveIndex].EvaluateAtParametric(rangeMap);
		}
	}
	return m_points[m_points.size() - 1];
}

Vec2 CubicHermiteSpline::GetVelocityAtParametric(float parametricZeroToOne, bool isPositive) const
{
	float deltaT = 1.f / (float)m_curves.size();
	float startT = 0.f;
	float endT = 0.f;
	for (int curveIndex = 0; curveIndex < (int)m_curves.size(); curveIndex++) {
		startT = endT;
		endT += deltaT;
		if (parametricZeroToOne >= startT && parametricZeroToOne <= endT) {
			float rangeMap = RangeMap(parametricZeroToOne, startT, endT, 0.f, 1.f);
			return m_curves[curveIndex].EvaluateVelAtParametric(rangeMap, isPositive);
		}
	}
	return m_curves[(int)m_curves.size() - 1].EvaluateVelAtParametric(1.f, isPositive);
}

float CubicHermiteSpline::GetApproximateLength(int numSubdivision) const
{
	float sum = 0.f;
	for (int curveIndex = 0; curveIndex < (int)m_curves.size(); curveIndex++) {
		sum += m_curves[curveIndex].GetApproximateLength(numSubdivision);
	}
	return sum;
}

Vec2 CubicHermiteSpline::EvaluateAtApproximateDistance(float distanceAlongCurve, int numSubdivisions) const
{
	float start = 0.f;
	float end = 0.f;
	for (int curveIndex = 0; curveIndex < (int)m_curves.size(); curveIndex++) {
		start = end;
		end += m_curves[curveIndex].GetApproximateLength(numSubdivisions);
		if (start <= distanceAlongCurve && distanceAlongCurve <= end) {
			return m_curves[curveIndex].EvaluateAtApproximateDistance(distanceAlongCurve - start, numSubdivisions);
		}
	}
	return m_curves[(int)m_curves.size() - 1].m_endPos;
}