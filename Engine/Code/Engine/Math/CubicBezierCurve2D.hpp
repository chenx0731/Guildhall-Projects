#pragma once
#include "Engine/Math/Vec2.hpp"
#include <vector>

struct Vertex_PCU;
struct Rgba8;

class CubicBezierCurve2D
{
public:
	CubicBezierCurve2D();
	CubicBezierCurve2D(Vec2 startPos, Vec2 guidePos1, Vec2 guidePos2, Vec2 endPos);
	CubicBezierCurve2D(CubicBezierCurve2D const& fromHermite);
	Vec2 EvaluateAtParametric(float parametricZeroToOne) const;
	float GetApproximateLength(int numSubdivision = 64) const;
	Vec2 EvaluateAtApproximateDistance(float distanceAlongCurve, int numSubdivisions = 64) const;

	Vec2 EvaluateVelAtParametric(float parametricZeroToOne, bool isPositive = true) const;

	void SetStartVelocity(Vec2 vel);
	void SetEndVelocity(Vec2 vel);

	void RecalculateCurve();

	void AddVertsForWholeCurve(std::vector<Vertex_PCU> &verts, float thickness, Rgba8 color, int numSubdivisions = 64) const;
	void AddVertsForCurveSeg(std::vector<Vertex_PCU> &verts, float parametricZeroToOne, float thickness, Rgba8 color, int numSubdivisions = 64) const;

public:
	Vec2 m_startPos;
	Vec2 m_guidePos1;
	Vec2 m_guidePos2;
	Vec2 m_endPos;

	Vec2 m_velocityA;
	Vec2 m_velocityD;
};

class CubicHermiteSpline
{
public:
	CubicHermiteSpline();
	CubicHermiteSpline(const std::vector<Vec2>& points);
	void AddVertsForSpline(std::vector<Vertex_PCU>& verts, float thickness, Rgba8 color, int numSubdivisions = 64) const;
	Vec2 EvaluateAtParametric(float parametricZeroToOne) const;
	Vec2 GetVelocityAtParametric(float parametricZeroToOne, bool isPositive = true) const;
	float GetApproximateLength(int numSubdivision = 64) const;
	Vec2 EvaluateAtApproximateDistance(float distanceAlongCurve, int numSubdivisions = 64) const;

public:
	std::vector<CubicBezierCurve2D> m_curves;
	std::vector<Vec2>				m_points;
	std::vector<Vec2>				m_vels;
};