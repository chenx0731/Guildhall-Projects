#define UNUSED(x) (void)(x);
#include "Engine/Core/DebugRenderSystem.hpp"
#include "Engine/Core/Stopwatch.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Math/EulerAngles.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Core/EventSystem.hpp"

#include <algorithm>
#include <mutex>

BitmapFont* s_theDebugRenderFont = nullptr;

enum class DebugRenderEntityType
{
	GEOMETRY,
	BILLBOARD,
	TEXT2D,
	MESSAGE
};


class DebugRenderEntity
{

public:
	DebugRenderEntity(float duration, const Rgba8& startColor, const Rgba8& endColor, DebugRenderMode mode);
	void Update();
	void Render(const Camera& camera) const;
	void ChangeColors(std::vector<Vertex_PCU>& verts, Rgba8 color) const;
	void ChangeColors(Rgba8 color);
	void UpdateCurrentColor();

	Mat44 GetModelMatrix() const;
	Rgba8 GetXRayColor() const;

public:
	std::vector<Vertex_PCU> m_verts;
	Stopwatch m_watch;
	Rgba8 m_startColor;
	Rgba8 m_endColor;
	Rgba8 m_currentColor;

	Vec3 m_position;
	EulerAngles m_orientation;
	Mat44 m_modelMatrix;

	DebugRenderMode m_debugRenderMode;
	DebugRenderEntityType m_type;
	BillboardType m_billboardType;

	std::string m_text;
	Vec2 m_alignment;

	bool m_isWireframe;
	bool m_isText;
};

bool Message_Sort(DebugRenderEntity* a, DebugRenderEntity* b)
{
	if (a->m_watch.m_duration != b->m_watch.m_duration)
		return a->m_watch.m_duration < b->m_watch.m_duration;
	else return a->m_watch.m_startTime < b->m_watch.m_startTime;
}

class DebugRender {
public:
	void Update();
	void RenderWorld(const Camera& camera) const;
	void RenderScreen(const Camera& camera) const;
public:
	DebugRenderConfig m_config;
	bool m_toggle = true;
	std::mutex						m_worldObjLock;
	std::vector<DebugRenderEntity*> m_worldObjects;
	std::mutex						m_screenObjLock;
	std::vector<DebugRenderEntity*> m_screenObjects;

};
static DebugRender* s_theDebugRender = nullptr;




void DebugRenderSystemStartup(const DebugRenderConfig& config)
{
	s_theDebugRender = new DebugRender();
	s_theDebugRender->m_config = config;
	s_theDebugRenderFont = s_theDebugRender->m_config.m_renderer->CreateOrGetBitmapFont("Data/Fonts/MyFixedFont");
	g_theEventSystem->SubscribeEventCallbackFunction("DebugRenderToggle", Command_DebugRenderToggle);
	g_theEventSystem->SubscribeEventCallbackFunction("DebugRenderClear", Command_DebugRenderClear);
}

void DebugRenderSystemShutdown()
{
	s_theDebugRender->m_screenObjLock.lock();
	s_theDebugRender->m_worldObjLock.lock();
	for (int entityIndex = 0; entityIndex < s_theDebugRender->m_screenObjects.size(); entityIndex++) {
		delete s_theDebugRender->m_screenObjects[entityIndex];
		s_theDebugRender->m_screenObjects[entityIndex] = nullptr;
	}
	for (int entityIndex = 0; entityIndex < s_theDebugRender->m_worldObjects.size(); entityIndex++) {
		delete s_theDebugRender->m_worldObjects[entityIndex];
		s_theDebugRender->m_worldObjects[entityIndex] = nullptr;
	}
	s_theDebugRender->m_screenObjLock.unlock();
	s_theDebugRender->m_worldObjLock.unlock();
	delete s_theDebugRender;
	s_theDebugRender = nullptr;
}

void DebugRenderSetVisible()
{
	s_theDebugRender->m_toggle = true;
}

void DebugRenderSetHidden()
{
	s_theDebugRender->m_toggle = false;
}

void DebugRenderClear()
{
	//s_theDebugRender->m_screenObjects.clear();

	s_theDebugRender->m_worldObjLock.lock();
	s_theDebugRender->m_worldObjects.clear();
	s_theDebugRender->m_worldObjLock.unlock();
}

void DebugRenderBeginFrame()
{
	s_theDebugRender->Update();
}

void DebugRenderWorld(const Camera& camera)
{
	if (!s_theDebugRender->m_toggle)
		return;
	s_theDebugRender->m_worldObjLock.lock();
	s_theDebugRender->m_config.m_renderer->BeginCamera(camera);
	s_theDebugRender->m_config.m_renderer->BindTexture(nullptr);
	s_theDebugRender->RenderWorld(camera);
	s_theDebugRender->m_config.m_renderer->EndCamera(camera);
	s_theDebugRender->m_worldObjLock.unlock();
}

void DebugRenderScreen(const Camera& camera)
{
	if (!s_theDebugRender->m_toggle)
		return;
	s_theDebugRender->m_screenObjLock.lock();
	s_theDebugRender->m_config.m_renderer->BeginCamera(camera);
	s_theDebugRender->m_config.m_renderer->BindTexture(nullptr);
	s_theDebugRender->RenderScreen(camera);
	s_theDebugRender->m_config.m_renderer->EndCamera(camera);
	s_theDebugRender->m_screenObjLock.unlock();
}

void DebugRenderEndFrame()
{
}

void DebugAddWorldPoint(const Vec3& pos, float radius, float duration, const Rgba8& startColor, const Rgba8& endColor, DebugRenderMode mode)
{
	s_theDebugRender->m_worldObjLock.lock();
	DebugRenderEntity* entity = new DebugRenderEntity(duration, startColor, endColor, mode);
	AddVertsForSphere3D(entity->m_verts, pos, radius);
	entity->m_type = DebugRenderEntityType::GEOMETRY;
	entity->m_watch.Start();
	s_theDebugRender->m_worldObjects.push_back(entity);
	s_theDebugRender->m_worldObjLock.unlock();
}

void DebugAddWorldLine(const Vec3& start, const Vec3& end, float radius, float duration, const Rgba8& startColor, const Rgba8& endColor, DebugRenderMode mode)
{
	s_theDebugRender->m_worldObjLock.lock();
	DebugRenderEntity* entity = new DebugRenderEntity(duration, startColor, endColor, mode);
	AddVertsForLineSegment3D(entity->m_verts, start, end, radius, startColor);
	entity->m_type = DebugRenderEntityType::GEOMETRY;
	entity->m_watch.Start();
	s_theDebugRender->m_worldObjects.push_back(entity);
	s_theDebugRender->m_worldObjLock.unlock();
}

void DebugAddWorldWireCylinder(const Vec3& base, const Vec3& top, float radius, float duration, const Rgba8& startColor, const Rgba8& endColor, DebugRenderMode mode)
{
	s_theDebugRender->m_worldObjLock.lock();
	DebugRenderEntity* entity = new DebugRenderEntity(duration, startColor, endColor, mode);
	AddVertsForCylinder3D(entity->m_verts, base, top, radius);
	entity->m_type = DebugRenderEntityType::GEOMETRY;
	entity->m_watch.Start();
	entity->m_isWireframe = true;
	s_theDebugRender->m_worldObjects.push_back(entity);
	s_theDebugRender->m_worldObjLock.unlock();
}

void DebugAddWorldWireSphere(const Vec3& center, float radius, float duration, const Rgba8& startColor, const Rgba8& endColor, DebugRenderMode mode)
{
	s_theDebugRender->m_worldObjLock.lock();
	DebugRenderEntity* entity = new DebugRenderEntity(duration, startColor, endColor, mode);
	AddVertsForSphere3D(entity->m_verts, center, radius);
	entity->m_type = DebugRenderEntityType::GEOMETRY;
	entity->m_watch.Start();
	entity->m_isWireframe = true;
	s_theDebugRender->m_worldObjects.push_back(entity);
	s_theDebugRender->m_worldObjLock.unlock();
}

void DebugAddWorldArrow(const Vec3& start, const Vec3& end, float radius, float duration, const Rgba8& startColor, const Rgba8& endColor, DebugRenderMode mode)
{
	s_theDebugRender->m_worldObjLock.lock();
	DebugRenderEntity* entity = new DebugRenderEntity(duration, startColor, endColor, mode);
	AddVertsForArrow3D(entity->m_verts, start, end, radius, startColor);
	entity->m_type = DebugRenderEntityType::GEOMETRY;
	entity->m_watch.Start();
	s_theDebugRender->m_worldObjects.push_back(entity);
	s_theDebugRender->m_worldObjLock.unlock();
}

void DebugAddWorldText(const std::string& text, const Vec3& origin, float textHeight, const Vec2& alignment, float duration, const Rgba8& startColor, const Rgba8& endColor, DebugRenderMode mode)
{
	s_theDebugRender->m_worldObjLock.lock();
	DebugRenderEntity* entity = new DebugRenderEntity(duration, startColor, endColor, mode);
	//Vec2 textMins = Vec2(origin.x, origin.y);
	s_theDebugRenderFont->AddVertsForText3D(entity->m_verts, Vec2(0.f, 0.f), textHeight, text, startColor, Mat44(), 0.7f, alignment);
	entity->m_type = DebugRenderEntityType::GEOMETRY;
	entity->m_position = origin;
	entity->m_isText = true;
	entity->m_watch.Start();
	s_theDebugRender->m_worldObjects.push_back(entity);
	s_theDebugRender->m_worldObjLock.unlock();
}

void DebugAddWorldBillboardText(const std::string& text, const Vec3& origin, float textHeight, const Vec2& alignment, float duration, const Rgba8& startColor, const Rgba8& endColor, DebugRenderMode mode)
{
	s_theDebugRender->m_worldObjLock.lock();
	DebugRenderEntity* entity = new DebugRenderEntity(duration, startColor, endColor, mode);
	Mat44 mat;
	mat.SetIJK3D(Vec3(0.f, 1.f, 0.f), Vec3(0.f, 0.f, 1.f), Vec3(1.f, 0.f, 0.f));
	s_theDebugRenderFont->AddVertsForText3D(entity->m_verts, Vec2(0.f, 0.f), textHeight, text, startColor, mat, 0.7f, alignment);
	entity->m_type = DebugRenderEntityType::BILLBOARD;
	entity->m_billboardType = BillboardType::FULL_CAMERA_OPPOSING;
	entity->m_position = origin;
	entity->m_isText = true;
	entity->m_watch.Start();
	s_theDebugRender->m_worldObjects.push_back(entity);
	s_theDebugRender->m_worldObjLock.unlock();
}

void DebugAddScreenText(const std::string& text, const AABB2& box, float size, const Vec2& alignment, float duration, const Rgba8& startColor, const Rgba8& endColor)
{
	s_theDebugRender->m_screenObjLock.lock();
	DebugRenderEntity* entity = new DebugRenderEntity(duration, startColor, endColor, DebugRenderMode::ALWAYS);
	//s_theDebugRenderFont->AddVertsForText2D(entity->m_verts, Vec2(0.f, 0.f), size, text, startColor, 0.7f);
	s_theDebugRenderFont->AddVertsForTextInBox2D(entity->m_verts, box, size, text, startColor, 0.7f, alignment);
	//AABB2 bound = GetVertexBounds2D(entity->m_verts);
	//Vec2 alignmentPos = Vec2(position.x - alignment.x * bound.GetDimensions().x, position.y - alignment.y * bound.GetDimensions().y);
	//TransformVertexArrayXY3D((int)entity->m_verts.size(), entity->m_verts.data(), 1.f, 0.f, alignmentPos);
	entity->m_type = DebugRenderEntityType::TEXT2D;
	entity->m_position = Vec2(0.f, 0.f);
	entity->m_isText = true;
	entity->m_watch.Start();
	s_theDebugRender->m_screenObjects.push_back(entity);
	s_theDebugRender->m_screenObjLock.unlock();
}

void DebugAddMessage(const std::string& text, float duration, const Rgba8& startColor, const Rgba8& endColor)
{
	s_theDebugRender->m_screenObjLock.lock();
	DebugRenderEntity* entity = new DebugRenderEntity(duration, startColor, endColor, DebugRenderMode::ALWAYS);
	//Vec2 textMins = Vec2(origin.x, origin.y);
	s_theDebugRenderFont->AddVertsForText2D(entity->m_verts, Vec2(0.f, 0.f), 20.f, text, Rgba8::WHITE, 0.7f);
	entity->m_type = DebugRenderEntityType::TEXT2D;
	entity->m_isText = true;
	entity->m_watch.Start();
	if (duration == 0.f)
		entity->m_position = Vec2(0.f, 775.f);
	else entity->m_position = Vec2(0.f, 750.f - (float)s_theDebugRender->m_screenObjects.size() * 25.f);
	s_theDebugRender->m_screenObjects.push_back(entity);
	s_theDebugRender->m_screenObjLock.unlock();
}

bool Command_DebugRenderClear(EventArgs& args)
{
	UNUSED(args);
	if (s_theDebugRender == nullptr)
		return false;
	DebugRenderClear();
	return true;
}

bool Command_DebugRenderToggle(EventArgs& args)
{
	UNUSED(args);
	if (s_theDebugRender == nullptr)
		return false;
	s_theDebugRender->m_toggle = !s_theDebugRender->m_toggle;
	return true;
}


DebugRenderEntity::DebugRenderEntity(float duration, const Rgba8& startColor, const Rgba8& endColor, DebugRenderMode mode)
{
	m_watch = Stopwatch(&Clock::GetSystemClock(), duration);
	m_startColor = startColor;
	m_endColor = endColor;
	m_debugRenderMode = mode;
	m_currentColor = startColor;
}

void DebugRenderEntity::Update()
{
	UpdateCurrentColor();
	ChangeColors(m_currentColor);
}

void DebugRenderEntity::Render(const Camera& camera) const
{
	if (m_isWireframe) {
		s_theDebugRender->m_config.m_renderer->SetRasterizerMode(RasterizerMode::WIREFRAME_CULL_BACK);
	}

	std::vector<Vertex_PCU> localVerts;
	localVerts = m_verts;

	switch (m_type)
	{
	case DebugRenderEntityType::GEOMETRY:
		s_theDebugRender->m_config.m_renderer->SetModelConstants();
		break;
	case DebugRenderEntityType::TEXT2D: {
		Vec2 pos = Vec2(m_position.x, m_position.y);
		TransformVertexArrayXY3D((int)localVerts.size(), localVerts.data(), 1.f, 0.f, pos);
		s_theDebugRender->m_config.m_renderer->SetModelConstants();
		break;
	}
	case DebugRenderEntityType::BILLBOARD: {
		Mat44 mat;
		Mat44 camMat = camera.m_orientation.
			GetMatrix_XFwd_YLeft_ZUp();
			//GetMatrix_YFwd_ZLeft_XUp();
		camMat.SetTranslation3D(camera.m_position);
		mat = GetBillboardMatrix(m_billboardType, camMat, m_position);
		s_theDebugRender->m_config.m_renderer->SetRasterizerMode(RasterizerMode::SOLID_CULL_NONE);
		s_theDebugRender->m_config.m_renderer->SetModelConstants(mat);
		break;
	}
	}
	s_theDebugRender->m_config.m_renderer->SetBlendMode(BlendMode::ALPHA);
	if (m_isText) {
		s_theDebugRender->m_config.m_renderer->BindTexture(&s_theDebugRenderFont->GetTexture());
	}
		

	switch (m_debugRenderMode)
	{
	case DebugRenderMode::ALWAYS:
		s_theDebugRender->m_config.m_renderer->SetDepthMode(DepthMode::DISABLED);
		s_theDebugRender->m_config.m_renderer->DrawVertexArray((int)localVerts.size(), localVerts.data());
		break;
	case DebugRenderMode::USE_DEPTH:
		s_theDebugRender->m_config.m_renderer->SetDepthMode(DepthMode::ENABLED);
		s_theDebugRender->m_config.m_renderer->DrawVertexArray((int)localVerts.size(), localVerts.data());
		break;
	case DebugRenderMode::X_RAY: {
		s_theDebugRender->m_config.m_renderer->SetDepthMode(DepthMode::DISABLED);
		s_theDebugRender->m_config.m_renderer->SetBlendMode(BlendMode::ALPHA);
		//s_theDebugRender->m_config.m_renderer->SetRasterizerMode(RasterizerMode::WIREFRAME_CULL_BACK);
		std::vector<Vertex_PCU> tempVerts;
		tempVerts = localVerts;
		Rgba8 tempColor = GetXRayColor();
		ChangeColors(tempVerts, tempColor);
		s_theDebugRender->m_config.m_renderer->DrawVertexArray((int)tempVerts.size(), tempVerts.data());

		s_theDebugRender->m_config.m_renderer->SetDepthMode(DepthMode::ENABLED);
		s_theDebugRender->m_config.m_renderer->SetBlendMode(BlendMode::OPAQUE);
		s_theDebugRender->m_config.m_renderer->DrawVertexArray((int)localVerts.size(), localVerts.data());

		break; 
	}
	}
	s_theDebugRender->m_config.m_renderer->BindTexture(nullptr);
	s_theDebugRender->m_config.m_renderer->SetRasterizerMode(RasterizerMode::SOLID_CULL_BACK);
	s_theDebugRender->m_config.m_renderer->SetDepthMode(DepthMode::ENABLED);
}

Mat44 DebugRenderEntity::GetModelMatrix() const
{
	Mat44 mat = m_orientation.GetMatrix_XFwd_YLeft_ZUp();
	mat.SetTranslation3D(m_position);
	return mat;
}

Rgba8 DebugRenderEntity::GetXRayColor() const
{
	//UpdateCurrentColor();
	Rgba8 color = m_currentColor;
	color.r += 20;
	color.g += 20;
	color.b += 20;
	color.a -= 100;
	if (color.r > 255)
		color.r = 255;
	if (color.g > 255)
		color.g = 255;
	if (color.b > 255)
		color.b = 255;
	if (color.a < 0)
		color.a = 0;
	return color;
}

void DebugRenderEntity::ChangeColors(std::vector<Vertex_PCU>& verts, Rgba8 color) const
{
	for (int vertsIndex = 0; vertsIndex < (int)verts.size(); vertsIndex++) {
		verts[vertsIndex].m_color = color;
	}
}

void DebugRenderEntity::ChangeColors(Rgba8 color)
{
	for (int vertsIndex = 0; vertsIndex < (int)m_verts.size(); vertsIndex++) {
		m_verts[vertsIndex].m_color = color;
	}
}

void DebugRenderEntity::UpdateCurrentColor()
{
	Rgba8 color;
	if (m_watch.m_duration <= 0.f) {
		m_currentColor = m_startColor;
		return;
	}
	float fraction = m_watch.GetElapsedFraction();
	float colorStart[4];
	float colorEnd[4];
	float colorCur[4];
	m_startColor.GetAsFloats(colorStart);
	m_endColor.GetAsFloats(colorEnd);
	colorCur[0] = Interpolate(colorStart[0], colorEnd[0], fraction);
	colorCur[1] = Interpolate(colorStart[1], colorEnd[1], fraction);
	colorCur[2] = Interpolate(colorStart[2], colorEnd[2], fraction);
	colorCur[3] = Interpolate(colorStart[3], colorEnd[3], fraction);
	color.SetAsFloats(colorCur);
	m_currentColor = color;
}

void DebugRender::Update()
{
	m_screenObjLock.lock();
	m_worldObjLock.lock();
	for (int entityIndex = 0; entityIndex < m_worldObjects.size(); entityIndex++) {
		bool isDelete = false;
		bool isShow = true;
		if (m_worldObjects[entityIndex]->m_watch.HasDurationElapsed() && m_worldObjects[entityIndex]->m_watch.m_duration >= 0.f) {
			isDelete = true;
			isShow = false;
		}
			
		if (isDelete) {
			DebugRenderEntity* temp = m_worldObjects[entityIndex];
			m_worldObjects[entityIndex] = m_worldObjects[(int)m_worldObjects.size() - 1];
			m_worldObjects[(int)m_worldObjects.size() - 1] = temp;
			m_worldObjects.pop_back();
		}
		if (isShow) {
			m_worldObjects[entityIndex]->Update();
		}
	}

	std::sort(m_screenObjects.begin(), m_screenObjects.end(), Message_Sort);

	Vec2 begin = Vec2(0.f, 750.f);
	for (int entityIndex = 0; entityIndex < m_screenObjects.size(); entityIndex++) {
		bool isDelete = false;
		bool isShow = true;
		if (m_screenObjects[entityIndex]->m_watch.HasDurationElapsed() && m_screenObjects[entityIndex]->m_watch.m_duration >= 0.f) {
			isDelete = true;
			isShow = false;
		}
		if (isDelete) {
			DebugRenderEntity* temp = m_screenObjects[entityIndex];
			m_screenObjects[entityIndex] = m_screenObjects[(int)m_screenObjects.size() - 1];
			m_screenObjects[(int)m_screenObjects.size() - 1] = temp;
			m_screenObjects.pop_back();
			entityIndex--;
		}
		if (isShow) {
			m_screenObjects[entityIndex]->m_position = begin;
			begin.y -= 25.f;
			m_screenObjects[entityIndex]->Update();
		}
	}
	m_screenObjLock.unlock();
	m_worldObjLock.unlock();
}

void DebugRender::RenderWorld(const Camera& camera) const
{
	/*
	std::vector<Vertex_PCU> arrowVerts;
	AddVertsForArrow3D(arrowVerts, Vec3(0.f, 0.f, 0.f), Vec3(2.f, 0.f, 0.f), 0.1f, Rgba8::RED);
	AddVertsForArrow3D(arrowVerts, Vec3(0.f, 0.f, 0.f), Vec3(0.f, 2.f, 0.f), 0.1f, Rgba8::GREEN);
	AddVertsForArrow3D(arrowVerts, Vec3(0.f, 0.f, 0.f), Vec3(0.f, 0.f, 2.f), 0.1f, Rgba8::BLUE);
	m_config.m_renderer->SetModelConstants();
	m_config.m_renderer->DrawVertexArray((int)arrowVerts.size(), arrowVerts.data());

	std::vector<Vertex_PCU> xtextVerts;
	Mat44 xBasis, yBasis, zBasis;
	xBasis.SetIJKT3D(Vec3(1.f, 0.f, 0.f), Vec3(0.f, 0.f, 1.f), Vec3(0.f, -1.f, 0.f), Vec3(0.5f, 0.f, 0.2f));
	yBasis.SetIJKT3D(Vec3(0.f, -1.f, 0.f), Vec3(0.f, 0.f, 1.f), Vec3(-1.f, 0.f, 0.f), Vec3(0.f, 1.5f, 0.2f));
	zBasis.SetIJKT3D(Vec3(0.f, 0.f, 1.f), Vec3(-1.f, 0.f, 0.f), Vec3(0.f, -1.f, 0.f), Vec3(0.5f, 0.f, 0.5f));
	*/
	//m_config.m_renderer->SetModelConstants(xBasis);
	/*
	m_config.m_renderer->SetRasterizerMode(RasterizerMode::SOLID_CULL_NONE);
	s_theDebugRenderFont->AddVertsForText3D(xtextVerts, Vec2(0.f, 0.f), 0.2f, "X - Forward", Rgba8::RED, xBasis, 0.7f, Vec2());
	s_theDebugRenderFont->AddVertsForText3D(xtextVerts, Vec2(0.f, 0.f), 0.2f, "Y - Left", Rgba8::GREEN, yBasis, 0.7f, Vec2());
	s_theDebugRenderFont->AddVertsForText3D(xtextVerts, Vec2(0.f, 0.f), 0.2f, "Z - Up", Rgba8::BLUE, zBasis, 0.7f, Vec2());
	m_config.m_renderer->BindTexture(&s_theDebugRenderFont->GetTexture());
	m_config.m_renderer->DrawVertexArray((int)xtextVerts.size(), xtextVerts.data());
	m_config.m_renderer->SetRasterizerMode(RasterizerMode::SOLID_CULL_BACK);
	m_config.m_renderer->BindTexture(nullptr);
	*/
	for (int entityIndex = 0; entityIndex < m_worldObjects.size(); entityIndex++) {
		//if (!m_worldObjects[entityIndex]->m_watch.HasDurationElapsed()) {
			m_worldObjects[entityIndex]->Render(camera);
		//}	
	}
}

void DebugRender::RenderScreen(const Camera& camera) const
{
	for (int entityIndex = 0; entityIndex < m_screenObjects.size(); entityIndex++) {
		//if (!m_screenObjects[entityIndex]->m_watch.HasDurationElapsed()) {
			m_screenObjects[entityIndex]->Render(camera);
		//}
	}
}
