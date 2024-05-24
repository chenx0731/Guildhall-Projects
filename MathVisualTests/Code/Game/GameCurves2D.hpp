#pragma once
#include "Game/GameCommon.hpp"
#include "Game/ShapeEntity2D.hpp"
#include "Game/Game.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Math/CubicBezierCurve2D.hpp"


class App;
class Entity;

struct Vec2;

enum class Easing
{
	Indentity,
	
	SmoothStart2,
	SmoothStart3,
	SmoothStart4,
	SmoothStart5,
	SmoothStart6,

	SmoothStop2,
	SmoothStop3,
	SmoothStop4,
	SmoothStop5,
	SmoothStop6,

	SmoothStep3,
	SmoothStep5,

	Hesitate3,
	Hesitate5,

	CustomFunky,

	Num
};

class GameCurves2D : public Game
{
public:
	GameCurves2D();
	virtual ~GameCurves2D();
	virtual void Startup() override;
	virtual void Update(float deltaSeconds) override;
	virtual void Render() const override;
	virtual void Shutdown() override;
private:

	void DeleteGarbageEntities();
	void KillAll();

	void UpdateEntities(float deltaSeconds);
	void UpdateFromKeyboard(float deltaSeconds);
	void UpdateFromController(float deltaSeconds);

	void RenderAttractMode() const;
	void RenderUI() const;
	void RenderEntities() const;
	void RenderPaused() const;
	void DebugRenderEntities() const;

	void RenderEasingCurve() const;
	void RenderBezierCurve() const;
	void RenderHermiteSpline() const;

	void AddVertsForEasingCurves(std::vector<Vertex_PCU> &verts, Easing type, float scale, float thickness, Rgba8 Color, float t = 1.f) const;

	void EnterGame();
	void SetCamera();

	void RandomizeShapes();
	
public:
	Vec2		m_cursor;
	int			m_numSubDivisions = 64;

	Easing		m_easingType = Easing::SmoothStart2;
	CubicBezierCurve2D m_curve;
	CubicHermiteSpline m_spline;
	float		m_time = 0.f;

	Rgba8		m_curveColor = Rgba8(50, 220, 50, 200);
	Rgba8		m_pointColor = Rgba8(50, 50, 200, 200);
	Rgba8		m_lineColor = Rgba8(50, 50, 200, 100);

	AABB2		m_easingArea;
	AABB2		m_curveArea;
	AABB2		m_splineArea;
};