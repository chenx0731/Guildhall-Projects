#define UNUSED(x) (void)(x);
#include "Game/GameCurves2D.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Entity.hpp"
#include "Game/App.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/Capsule2.hpp"
#include "Engine/Math/OBB2.hpp"
#include "Engine/Math/LineSegment2.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"

GameCurves2D::GameCurves2D() : Game()
{
	m_easingArea = AABB2(Vec2(10.f, 55.f), Vec2(90.f, 90.f));
	m_curveArea = AABB2(Vec2(110.f, 55.f), Vec2(190.f, 90.f));
	m_splineArea = AABB2(Vec2(10.f, 5.f), Vec2(190.f, 45.f));
	RandomizeShapes();
	/*Vec2 start = Vec2(g_rng->RollRandomFloatInRange(120.f, 150.f), g_rng->RollRandomFloatInRange(60.f, 70.f));
	Vec2 end = start + Vec2(g_rng->RollRandomFloatInRange(10.f, 20.f), g_rng->RollRandomFloatInRange(10.f, 20.f));
	Vec2 startVel = Vec2(g_rng->RollRandomFloatInRange(-30.f, 30.f), g_rng->RollRandomFloatInRange(-30.f, 30.f));
	Vec2 endVel = Vec2(g_rng->RollRandomFloatInRange(-30.f, 30.f), g_rng->RollRandomFloatInRange(-30.f, 30.f));

	m_curve = CubicBezierCurve2D(start, Vec2(), Vec2(), end);
	m_curve.m_velocityA = startVel;
	m_curve.m_velocityD = endVel;

	m_curve.RecalculateCurve();*/
}

GameCurves2D::~GameCurves2D()
{

}

void GameCurves2D::Startup()
{
	Vec2 worldCenter(WORLD_CENTER_X, WORLD_CENTER_Y);

	SetCamera();

	g_theRenderer->SetDepthMode(DepthMode::DISABLED);
	g_theRenderer->SetRasterizerMode(RasterizerMode::SOLID_CULL_NONE);
}

void GameCurves2D::Update(float deltaSeconds)
{
	UpdateFromKeyboard(deltaSeconds);
	UpdateFromController(deltaSeconds);
	UpdateEntities(deltaSeconds);
	SetCamera();

	if (m_isAttractMode)
	{
		return;
	}


	if (m_isOver)
	{
		m_hangingTime += deltaSeconds;
		if (m_hangingTime >= 3.f)
		{
			ReturnAttractModeVictory();
		}
	}
	
	if (!m_isPaused)
	{
		UpdateEntities(deltaSeconds);

		DeleteGarbageEntities();
	}

	
}

void GameCurves2D::Render() const
{
	g_theRenderer->BeginCamera(m_worldCamera);

	RenderEasingCurve();
	RenderBezierCurve();
	RenderHermiteSpline();

	std::vector<Vertex_PCU> fontVerts;
	std::string title = "Mode (F6/F7 for prev/next): Easing, Curves, Splines(2D)";
	std::string controls = Stringf("F8 to randomize; W/E = prev/next Easing function; N/M = curve subdivisions (%d), hold T = slow", m_numSubDivisions);
	g_theFont->AddVertsForTextInBox2D(fontVerts, AABB2(0.f, 90.f, 200.f, 100.f), 2.5f, title.c_str(), Rgba8(200, 200, 0), 0.7f, Vec2(0.01f, 0.7f));
	g_theFont->AddVertsForTextInBox2D(fontVerts, AABB2(0.f, 77.f, 200.f, 100.f), 2.2f, controls.c_str(), Rgba8(0, 200, 100), 0.7f, Vec2(0.02f, 0.7f));
	g_theRenderer->BindTexture(&g_theFont->GetTexture());
	//AddVertsForTextTriangles2D(fontVerts, "Raycast vs. Discs 2D", Vec2(5.f, 92.f), 3.f, Rgba8(255, 255, 0));
	g_theRenderer->DrawVertexArray((int)fontVerts.size(), fontVerts.data());
	g_theRenderer->BindTexture(nullptr);
	g_theRenderer->EndCamera(m_worldCamera);
}

void GameCurves2D::Shutdown()
{
	for (int entityIndex = 0; entityIndex < m_shapes.size(); entityIndex++) {
		if (m_shapes[entityIndex]) {
			delete m_shapes[entityIndex];
			m_shapes[entityIndex] = nullptr;
		}
	}
}

void GameCurves2D::UpdateEntities(float deltaSeconds)
{
	//UNUSED(deltaSeconds);
	m_time += deltaSeconds;
}

void GameCurves2D::DeleteGarbageEntities()
{

}

void GameCurves2D::KillAll()
{

}

void GameCurves2D::UpdateFromKeyboard(float deltaSeconds)
{
	UNUSED(deltaSeconds);
	if (g_theInput->IsKeyDown(KEYCODE_UPARROW))
	{
		m_cursor.y += deltaSeconds * 20.f;
	}
	if (g_theInput->IsKeyDown(KEYCODE_DOWNARROW))
	{
		m_cursor.y -= deltaSeconds * 20.f;
	}
	if (g_theInput->IsKeyDown(KEYCODE_LEFTARROW))
	{
		m_cursor.x -= deltaSeconds * 20.f;
	}
	if (g_theInput->IsKeyDown(KEYCODE_RIGHTARROW))
	{
		m_cursor.x += deltaSeconds * 20.f;
	}
	if (g_theInput->WasKeyJustPressed('P'))
	{
		m_isPaused = !m_isPaused;
	}
	if (g_theInput->WasKeyJustPressed('O'))
	{
		m_isPaused = true;
		SetCamera();

		if (m_isAttractMode)
		{
			return;
		}

		UpdateEntities(deltaSeconds);

		DeleteGarbageEntities();

		if (m_isShaking)
		{
			float theta = g_rng->RollRandomFloatInRange(0.f, 360.f);
			float r = g_rng->RollRandomFloatInRange(0.f, 5.f * (1.f - m_shakingTime));
			m_worldCamera.Translate2D(Vec2::MakeFromPolarDegrees(theta, r));
			m_shakingTime += deltaSeconds;
			if (m_shakingTime >= 1.f)
			{
				m_shakingTime = 0.f;
				m_isShaking = false;
			}
		}
	}
	if (g_theInput->WasKeyJustPressed(KEYCODE_F1))
	{
		m_isDebug = !m_isDebug;
	}
	if (g_theInput->WasKeyJustPressed(KEYCODE_F8))
	{
		RandomizeShapes();
	}
	if (g_theInput->WasKeyJustPressed('K'))
	{
		KillAll();
	}
	if (g_theInput->WasKeyJustPressed('W')) {
		int index = (int)m_easingType;
		index--;
		if (index < 0)
			index = (int)(Easing::Num) - 1;
		m_easingType = (Easing)index;
	}
	if (g_theInput->WasKeyJustPressed('E')) {
		int index = (int)m_easingType;
		index++;
		if (index >= (int)Easing::Num)
			index = 0;
		m_easingType = (Easing)index;
	}
	if (g_theInput->WasKeyJustPressed('N')) {
		if (m_numSubDivisions >= 1)
			m_numSubDivisions /= 2;
	}
	if (g_theInput->WasKeyJustPressed('M')) {
		m_numSubDivisions *= 2;
	}
}

void GameCurves2D::UpdateFromController(float deltaSeconds)
{
	UNUSED(deltaSeconds);
	if (g_theInput->GetController(0).WasButtonJustPressed(XBOX_BUTTON_START) 
		|| g_theInput->GetController(0).WasButtonJustPressed(XBOX_BUTTON_A))
	{
		if (m_isAttractMode)
		{
			EnterGame();
		}
	}
}

void GameCurves2D::RenderAttractMode() const
{
	g_theRenderer->BeginCamera(m_screenCamera);
	
	DebugDrawRing(Vec2(SCREEN_CAMERA_SIZE_X * 0.5f, SCREEN_CAMERA_SIZE_Y * 0.5),
		(150.f + 50.f * SinDegrees(100.f * static_cast<float>(GetCurrentTimeSeconds()))),
			(20.f + 5.f * CosDegrees(100.f * static_cast<float>(GetCurrentTimeSeconds()))),
				Rgba8(255, 150, 0));

	g_theRenderer->EndCamera(m_screenCamera);
}

void GameCurves2D::RenderUI() const
{
	g_theRenderer->BeginCamera(m_screenCamera);

	g_theRenderer->EndCamera(m_screenCamera);
}


void GameCurves2D::RenderEntities() const
{
	g_theRenderer->BeginCamera(m_worldCamera);

	g_theRenderer->ClearScreen(Rgba8(0, 0, 0));

	g_theRenderer->EndCamera(m_worldCamera);
}

void GameCurves2D::RenderPaused() const
{
	g_theRenderer->BeginCamera(m_screenCamera);
	DrawSquare(m_screenCamera.m_bottomLeft, m_screenCamera.m_topRight, Rgba8(127, 127, 127, 127));
	g_theRenderer->EndCamera(m_screenCamera);
}

void GameCurves2D::DebugRenderEntities() const
{
	g_theRenderer->BeginCamera(m_worldCamera);
	g_theRenderer->EndCamera(m_worldCamera);
}

void GameCurves2D::RenderEasingCurve() const
{
	std::vector<Vertex_PCU> easingCurve, pointAndLines, texts, background;
	std::string string = "";

	AABB2 box = AABB2();
	box.SetCenter(Vec2(50.f, 70.f));
	box.SetDimensions(Vec2(30.f, 30.f));
	AABB2 textBox;
	textBox.SetCenter(Vec2(box.GetCenter().x, box.m_mins.y));
	textBox.SetDimensions(Vec2(box.GetDimensions().x, 5.f));

	Vec2 end;
	float time = fmodf(m_time, 2.f);
	Easing m_type = m_easingType;
	float endX = time / 2.f;
	float scale = box.GetDimensions().x;
	float thickness = 0.5f;
	if (m_type == Easing::Indentity) {
		end = Vec2(endX * scale, endX * scale);
		string = "Indentity";
	}
	if (m_type == Easing::SmoothStart2) {
		end = Vec2(endX * scale, SmoothStart2(endX) * scale);
		string = "SmoothStart2";
	}
	if (m_type == Easing::SmoothStart3) {
		end = Vec2(endX * scale, SmoothStart3(endX) * scale);
		string = "SmoothStart3";
	}
	if (m_type == Easing::SmoothStart4) {
		end = Vec2(endX * scale, SmoothStart4(endX) * scale);
		string = "SmoothStart4";
	}
	if (m_type == Easing::SmoothStart5) {
		end = Vec2(endX * scale, SmoothStart5(endX) * scale);
		string = "SmoothStart5";
	}
	if (m_type == Easing::SmoothStart6) {
		end = Vec2(endX * scale, SmoothStart6(endX) * scale);
		string = "SmoothStart6";
	}

	if (m_type == Easing::SmoothStop2) {
		end = Vec2(endX * scale, SmoothStop2(endX) * scale);
		string = "SmoothStop2";
	}
	if (m_type == Easing::SmoothStop3) {
		end = Vec2(endX * scale, SmoothStop3(endX) * scale);
		string = "SmoothStop3";
	}
	if (m_type == Easing::SmoothStop4) {
		end = Vec2(endX * scale, SmoothStop4(endX) * scale);
		string = "SmoothStop4";
	}
	if (m_type == Easing::SmoothStop5) {
		end = Vec2(endX * scale, SmoothStop5(endX) * scale);
		string = "SmoothStop5";
	}
	if (m_type == Easing::SmoothStop6) {
		end = Vec2(endX * scale, SmoothStop6(endX) * scale);
		string = "SmoothStop6";
	}

	if (m_type == Easing::SmoothStep3) {
		end = Vec2(endX * scale, SmoothStep3(endX) * scale);
		string = "SmoothStep3";
	}
	if (m_type == Easing::SmoothStep5) {
		end = Vec2(endX * scale, SmoothStep5(endX) * scale);
		string = "SmoothStep5";
	}

	if (m_type == Easing::Hesitate3) {
		end = Vec2(endX * scale, Hesitate3(endX) * scale);
		string = "Hesitate3";
	}
	if (m_type == Easing::Hesitate5) {
		end = Vec2(endX * scale, Hesitate5(endX) * scale);
		string = "Hesitate5";
	}
	if (m_type == Easing::CustomFunky){ 
		end = Vec2(endX * scale, CustomFunky(endX) * scale);
		string = "CustomFuncky";
	}

	AddVertsForAABB2D(background, box, Rgba8(50, 50, 200, 50));
	AddVertsForEasingCurves(easingCurve, m_easingType, scale, thickness, Rgba8(50, 255, 50, 200));
	AddVertsForLineSegment2D(pointAndLines, Vec2(end.x, 0.f), end, thickness, Rgba8::GREY);
	AddVertsForLineSegment2D(pointAndLines, Vec2(0.f, end.y), end, thickness, Rgba8::GREY);
	AddVertsForDisc2D(pointAndLines, end, 0.7f, Rgba8::WHITE);
	g_theFont->AddVertsForTextInBox2D(texts, textBox, 10.f, string);
	
	TransformVertexArrayXY3D((int)easingCurve.size(), easingCurve.data(), 1.f, 0.f, box.m_mins);
	TransformVertexArrayXY3D((int)pointAndLines.size(), pointAndLines.data(), 1.f, 0.f, box.m_mins);
	g_theRenderer->DrawVertexArray((int)background.size(), background.data());
	g_theRenderer->DrawVertexArray((int)easingCurve.size(), easingCurve.data());
	g_theRenderer->DrawVertexArray((int)pointAndLines.size(), pointAndLines.data());

	g_theRenderer->BindTexture(&g_theFont->GetTexture());
	g_theRenderer->DrawVertexArray((int)texts.size(), texts.data());
	g_theRenderer->BindTexture(nullptr);
}

void GameCurves2D::RenderBezierCurve() const
{
	std::vector<Vertex_PCU> bezierCurve, bezierGrey;

	m_curve.AddVertsForWholeCurve(bezierGrey, 0.5f, Rgba8::GREY);
	//m_spline.AddVertsForSpline(splineGrey, 0.5f, Rgba8::GREY);
	g_theRenderer->DrawVertexArray((int)bezierGrey.size(), bezierGrey.data());
	m_curve.AddVertsForWholeCurve(bezierCurve, 0.5f, m_curveColor, m_numSubDivisions);
	g_theRenderer->DrawVertexArray((int)bezierCurve.size(), bezierCurve.data());

	float time = fmodf(m_time, 2.f);
	float percentageT = time / 2.f;

	Vec2 white = m_curve.EvaluateAtParametric(percentageT);

	float length = m_curve.GetApproximateLength();
	float speed = length / 2.f;
	Vec2 green = m_curve.EvaluateAtApproximateDistance(speed * time, m_numSubDivisions);

	DrawCircle(m_curve.m_startPos, 0.7f, m_pointColor);
	DrawCircle(m_curve.m_guidePos1, 0.7f, m_pointColor);
	DrawCircle(m_curve.m_guidePos2, 0.7f, m_pointColor);
	DrawCircle(m_curve.m_endPos, 0.7f, m_pointColor);

	DrawCircle(white, 0.7f, Rgba8::WHITE);
	DrawCircle(green, 0.7f, Rgba8(0, 200, 0));

	DebugDrawLine(m_curve.m_startPos, m_curve.m_guidePos1, 0.3f, m_lineColor);
	DebugDrawLine(m_curve.m_guidePos1, m_curve.m_guidePos2, 0.3f, m_lineColor);
	DebugDrawLine(m_curve.m_guidePos2, m_curve.m_endPos, 0.3f, m_lineColor);

	
}

void GameCurves2D::RenderHermiteSpline() const
{
	std::vector<Vertex_PCU> spline, splineGrey;
	m_spline.AddVertsForSpline(spline, 0.5f, m_curveColor, m_numSubDivisions);
	m_spline.AddVertsForSpline(splineGrey, 0.5f, Rgba8::GREY);
	g_theRenderer->DrawVertexArray((int)splineGrey.size(), splineGrey.data());
	g_theRenderer->DrawVertexArray((int)spline.size(), spline.data());

	float time = fmodf(m_time, 2.f * (float)m_spline.m_curves.size());
	float percentageT = time / (2.f * (float)m_spline.m_curves.size());

	Vec2 white = m_spline.EvaluateAtParametric(percentageT);

	float length = m_spline.GetApproximateLength();
	float speed = length / (2.f * (float)m_spline.m_curves.size());
	Vec2 green = m_spline.EvaluateAtApproximateDistance(speed * time, m_numSubDivisions);
	
	DrawCircle(white, 0.7f, Rgba8::WHITE);
	DrawCircle(green, 0.7f, Rgba8(0, 200, 0));

	for (int curveIndex = 0; curveIndex < m_spline.m_curves.size(); curveIndex++) {
		if (curveIndex == 0)
			DrawCircle(m_spline.m_curves[curveIndex].m_startPos, 0.7f, m_pointColor);
		DrawCircle(m_spline.m_curves[curveIndex].m_endPos, 0.7f, m_pointColor);
		std::vector<Vertex_PCU> arrow;
		AddVertsForArrow2D(arrow, m_spline.m_curves[curveIndex].m_endPos,
			m_spline.m_curves[curveIndex].m_endPos + m_spline.m_curves[curveIndex].m_velocityD, 1.f, 0.5f, Rgba8::RED);
		g_theRenderer->DrawVertexArray((int)arrow.size(), arrow.data());
		DebugDrawLine(m_spline.m_curves[curveIndex].m_startPos, m_spline.m_curves[curveIndex].m_endPos, 0.5f, m_lineColor);
	}
}

void GameCurves2D::AddVertsForEasingCurves(std::vector<Vertex_PCU>& verts, Easing type, float scale, float thickness, Rgba8 Color, float t) const
{
	float deltaT = 1.f / (float)m_numSubDivisions;
	float startX = 0.f;
	for (int divisionIndex = 0; divisionIndex < m_numSubDivisions; divisionIndex++) {
		float endX = startX + deltaT;
		if (startX <= t && t <= endX)
			endX = t;
		Vec2 start, end;
		if (type == Easing::Indentity) {
			start = Vec2(startX * scale, startX * scale);
			end = Vec2(endX * scale, endX * scale);
		}
		if (type == Easing::SmoothStart2) {
			start = Vec2(startX * scale, SmoothStart2(startX) * scale);
			end = Vec2(endX * scale, SmoothStart2(endX) * scale);
		}
		if (type == Easing::SmoothStart3) {
			start = Vec2(startX * scale, SmoothStart3(startX) * scale);
			end = Vec2(endX * scale, SmoothStart3(endX) * scale);
		}
		if (type == Easing::SmoothStart4) {
			start = Vec2(startX * scale, SmoothStart4(startX) * scale);
			end = Vec2(endX * scale, SmoothStart4(endX) * scale);
		}
		if (type == Easing::SmoothStart5) {
			start = Vec2(startX * scale, SmoothStart5(startX) * scale);
			end = Vec2(endX * scale, SmoothStart5(endX) * scale);
		}
		if (type == Easing::SmoothStart6) {
			start = Vec2(startX * scale, SmoothStart6(startX) * scale);
			end = Vec2(endX * scale, SmoothStart6(endX) * scale);
		}

		if (type == Easing::SmoothStop2) {
			start = Vec2(startX * scale, SmoothStop2(startX) * scale);
			end = Vec2(endX * scale, SmoothStop2(endX) * scale);
		}
		if (type == Easing::SmoothStop3) {
			start = Vec2(startX * scale, SmoothStop3(startX) * scale);
			end = Vec2(endX * scale, SmoothStop3(endX) * scale);
		}
		if (type == Easing::SmoothStop4) {
			start = Vec2(startX * scale, SmoothStop4(startX) * scale);
			end = Vec2(endX * scale, SmoothStop4(endX) * scale);
		}
		if (type == Easing::SmoothStop5) {
			start = Vec2(startX * scale, SmoothStop5(startX) * scale);
			end = Vec2(endX * scale, SmoothStop5(endX) * scale);
		}
		if (type == Easing::SmoothStop6) {
			start = Vec2(startX * scale, SmoothStop6(startX) * scale);
			end = Vec2(endX * scale, SmoothStop6(endX) * scale);
		}

		if (type == Easing::SmoothStep3) {
			start = Vec2(startX * scale, SmoothStep3(startX) * scale);
			end = Vec2(endX * scale, SmoothStep3(endX) * scale);
		}
		if (type == Easing::SmoothStep5) {
			start = Vec2(startX * scale, SmoothStep5(startX) * scale);
			end = Vec2(endX * scale, SmoothStep5(endX) * scale);
		}

		if (type == Easing::Hesitate3) {
			start = Vec2(startX * scale, Hesitate3(startX) * scale);
			end = Vec2(endX * scale, Hesitate3(endX) * scale);
		}
		if (type == Easing::Hesitate5) {
			start = Vec2(startX * scale, Hesitate5(startX) * scale);
			end = Vec2(endX * scale, Hesitate5(endX) * scale);
		}

		if (type == Easing::CustomFunky) {
			start = Vec2(startX * scale, CustomFunky(startX) * scale);
			end = Vec2(endX * scale, CustomFunky(endX) * scale);
		}
		AddVertsForLineSegment2D(verts, LineSegment2(start, end), thickness, Color);
		if (endX == t)
			break;
		startX = endX;
	}
}


void GameCurves2D::EnterGame()
{
	Shutdown();
	Startup();
	m_isAttractMode = false;
	g_theInput->EndFrame();
}

void GameCurves2D::SetCamera()
{
	m_worldCamera.SetOrthoView(Vec2(0.f, 0.f), Vec2(WORLD_CAMERA_SIZE_X, WORLD_CAMERA_SIZE_Y));
	m_screenCamera.SetOrthoView(Vec2(0.f, 0.f), Vec2(SCREEN_CAMERA_SIZE_X, SCREEN_CAMERA_SIZE_Y));
}

void GameCurves2D::RandomizeShapes()
{
	Vec2 start = Vec2(g_rng->RollRandomFloatInRange(m_curveArea.m_mins.x + 5.f, m_curveArea.m_maxs.x - 5.f), g_rng->RollRandomFloatInRange(m_curveArea.m_mins.y + 5.f, m_curveArea.m_maxs.y - 5.f));
		//Vec2(g_rng->RollRandomFloatInRange(120.f, 150.f), g_rng->RollRandomFloatInRange(60.f, 70.f));
	Vec2 end = Vec2(g_rng->RollRandomFloatInRange(m_curveArea.m_mins.x + 5.f, m_curveArea.m_maxs.x - 5.f), g_rng->RollRandomFloatInRange(m_curveArea.m_mins.y + 5.f, m_curveArea.m_maxs.y - 5.f));
	Vec2 startVel = Vec2(g_rng->RollRandomFloatInAbsRange(20.f, 40.f), g_rng->RollRandomFloatInAbsRange(20.f, 40.f));
	Vec2 endVel = Vec2(g_rng->RollRandomFloatInAbsRange(20.f, 40.f), g_rng->RollRandomFloatInAbsRange(20.f, 40.f));

	m_curve = CubicBezierCurve2D(start, Vec2(), Vec2(), end);
	m_curve.m_velocityA = startVel;
	m_curve.m_velocityD = endVel;

	m_curve.RecalculateCurve();
	m_easingType = (Easing)g_rng->RollRandomIntInRange(0, (int)Easing::Num - 1);
	int pointsNum = g_rng->RollRandomIntInRange(4, 6);
	std::vector<Vec2> points;
	//float x = g_rng->RollRandomFloatInRange(20.f, 30.f);
	//float y = g_rng->RollRandomFloatInRange(10.f, 20.f);
	//start = Vec2(x, y);
	float deltaPercent = 1.f / (float)(pointsNum);
	for (int pointIndex = 0; pointIndex < pointsNum; pointIndex++) {
		//float delX = g_rng->RollRandomFloatInAbsRange(20.f, 30.f);
		//float delY = g_rng->RollRandomFloatInAbsRange(5.f, 15.f);
		float percent = (float)pointIndex / pointsNum;

		Vec2 point = Vec2(g_rng->RollRandomFloatInRange(m_splineArea.m_mins.x + percent * m_splineArea.GetDimensions().x, m_splineArea.m_mins.x + (percent + deltaPercent) * m_splineArea.GetDimensions().x),
			g_rng->RollRandomFloatInRange(m_splineArea.m_mins.y, m_splineArea.m_maxs.y));
			//start + Vec2(delX, delY);
		points.push_back(point);
		start = point;
	}

	m_spline = CubicHermiteSpline(points);
}


