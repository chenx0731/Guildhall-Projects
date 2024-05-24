#pragma once
#include "Game/GameCommon.hpp"
#include "Game/Game.hpp"
#include "Game/ConvexSceneObject.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Core/BufferWriter.hpp"
#include "Engine/Core/BufferParser.hpp"

class App;
class Entity;
class Player;
class Prop;
class Clock;
class Shader;
class Texture;

struct RaycastResult2D;
struct Vec2;


class GameBasic2D : public Game
{
public:
	GameBasic2D();
	virtual ~GameBasic2D();
	virtual void Startup() override;
	virtual void Update() override;
	virtual void Render() const override;
	virtual void Shutdown() override;

	virtual void Enter() override;
	virtual void Exit() override;

private:

	void LoadAssets();
	
	void DeleteGarbageEntities();
	void KillAll();

	void UpdateEntities();
	void UpdateFromKeyboard();
	void UpdateFromController();

	RaycastResult2D RaycastVsObjects(Vec2 start, Vec2 fwdNormal, float maxDist) const;

	void RenderAttractMode() const;
	void RenderUI() const;
	void RenderEntities() const;
	void RenderPaused() const;
	void DebugRenderEntities() const;
	void RenderGrid() const;

	void EnterGame();
	void SetCamera();

	void CreateDiscTree();
	
public:

	Camera		m_worldCamera;
	Camera		m_screenCamera;

	bool		m_isDebugBoundingDisc = false;
	bool		m_isDebugPS = false;
	bool		m_isPaused = false;
	bool		m_isShaking = false;
	bool		m_isAttractMode = false;
	bool		m_isOver = false;

	bool		m_isOptimized = true;
	bool		m_isTreeUpdated = true;

	bool		m_isTransparent = true;

	bool		m_isRaytesting = false;

	bool		m_isDraggingObj = false;

	float		m_hangingTime = 0.f;
	int			m_current_level = 0;
	float		m_shakingTime = 0.f;

	Player*		m_player;
	Prop*		m_prop1;
	Prop*		m_prop2;
	Prop*		m_sphere;
	Shader*		m_shader = nullptr;

	Texture*	m_natureBackground = nullptr;

	Clock*		m_clock;

	std::vector<ConvexSceneObject> m_objects;

	Vec2		m_cursor;
	Vec2		m_previousCursor;
	Vec2		m_arrowStart = Vec2(0.f, 0.f);
	Vec2		m_arrowEnd = Vec2(0.f, 0.f);
	bool		m_didImpact = false;
	Vec2		m_impactPos = Vec2(0.f, 0.f);
	Vec2		m_impactNormal = Vec2(0.f, 0.f);

	bool		m_setStart = false;
	bool		m_setEnd = false;

	int			m_numOfObjects = 8;
	int			m_numOfRay = 1024;

	AABB2		m_worldBound = AABB2(Vec2(0.f, 0.f), Vec2(200.f, 100.f));

	std::vector<unsigned char> m_buffer;

	std::vector<Disc2TreeNode> m_disc2Tree;
	
	int			m_focusedPolyIndex = -1;
};