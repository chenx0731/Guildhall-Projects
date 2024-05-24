#pragma once
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Core/EventSystem.hpp"
#include <string>
#include <vector>

class Renderer;
class Camera;
//class EventArgs;
struct Vec2;
struct Vec3;
struct Vertex_PCU;
struct Mat44;

enum class DebugRenderMode
{
	ALWAYS,
	USE_DEPTH,
	X_RAY,
};

struct DebugRenderConfig
{
	Renderer* m_renderer = nullptr;
	bool m_startHidden = false;
};

// Setup
void DebugRenderSystemStartup(const DebugRenderConfig& config);
void DebugRenderSystemShutdown();

// Control
void DebugRenderSetVisible();
void DebugRenderSetHidden();
void DebugRenderClear();

// Output
void DebugRenderBeginFrame();
void DebugRenderWorld(const Camera& camera);
void DebugRenderScreen(const Camera& camera);
void DebugRenderEndFrame();

// Geometry
void DebugAddWorldPoint(const Vec3& pos,
	float radius, float duration,
	const Rgba8& startColor = Rgba8::WHITE,
	const Rgba8& endColor = Rgba8::WHITE,
	DebugRenderMode mode = DebugRenderMode::USE_DEPTH);
void DebugAddWorldLine(const Vec3& start, const Vec3& end,
	float radius, float duration,
	const Rgba8& startColor = Rgba8::WHITE,
	const Rgba8& endColor = Rgba8::WHITE,
	DebugRenderMode mode = DebugRenderMode::USE_DEPTH);
void DebugAddWorldWireCylinder(const Vec3& base, const Vec3& top,
	float radius, float duration,
	const Rgba8& startColor = Rgba8::WHITE,
	const Rgba8& endColor = Rgba8::WHITE,
	DebugRenderMode mode = DebugRenderMode::USE_DEPTH);
void DebugAddWorldWireSphere(const Vec3& center,
	float radius, float duration,
	const Rgba8& startColor = Rgba8::WHITE,
	const Rgba8& endColor = Rgba8::WHITE,
	DebugRenderMode mode = DebugRenderMode::USE_DEPTH);
void DebugAddWorldArrow(const Vec3& start, const Vec3& end,
	float radius, float duration,
	const Rgba8& startColor = Rgba8::WHITE,
	const Rgba8& endColor = Rgba8::WHITE,
	DebugRenderMode mode = DebugRenderMode::USE_DEPTH);
void DebugAddWorldText(const std::string& text,
	const Vec3& origin, float textHeight,
	const Vec2& alignment, float duration,
	const Rgba8& startColor = Rgba8::WHITE,
	const Rgba8& endColor = Rgba8::WHITE,
	DebugRenderMode mode = DebugRenderMode::USE_DEPTH);
void DebugAddWorldBillboardText(const std::string& text,
	const Vec3& origin, float textHeight,
	const Vec2& alignment, float duration,
	const Rgba8& startColor = Rgba8::WHITE,
	const Rgba8& endColor = Rgba8::WHITE,
	DebugRenderMode mode = DebugRenderMode::USE_DEPTH);
void DebugAddScreenText(const std::string& text,
	const AABB2& box, float size,
	const Vec2& alignment, float duration,
	const Rgba8& startColor = Rgba8::WHITE,
	const Rgba8& endColor = Rgba8::WHITE);
void DebugAddMessage(const std::string& text, float duration,
	const Rgba8& startColor = Rgba8::WHITE,
	const Rgba8& endColor = Rgba8::WHITE);

bool Command_DebugRenderClear(EventArgs& args);
bool Command_DebugRenderToggle(EventArgs& args);
