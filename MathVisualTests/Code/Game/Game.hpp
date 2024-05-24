#pragma once
#include "Game/GameCommon.hpp"
#include "Game/ShapeEntity2D.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Core/Vertex_PCU.hpp"


class App;
class Entity;

struct Vec2;
//struct AABB2;
//struct OBB2;
//struct Capsule2;
//struct LineSegment2;

enum GameStatus
{
	GAME_STATUS_NULL = -1,

	GAME_STATUS_NEAREST_POINT,
	GAME_STATUS_RAYCAST_DISC_2D,
	GAME_STATUS_BILLIARDS_2D,
	GAME_STATUS_TEST_SHAPE_3D,
	GAME_STATUS_RAYCAST_LINE_2D,
	GAME_STATUS_RAYCAST_AABB_2D,
	GAME_STATUS_RAYCAST_CONVEX,
	GAME_STATUS_PACHINKO_2D,
	GAME_STATUS_CURVES_2D,

	GAME_STATUS_NUM
};


class Game
{
public:
	Game();
	virtual ~Game();
	virtual void Startup() {};
	virtual void Update(float deltaSeconds)	= 0;
	virtual void Render() const = 0;
	virtual void Shutdown() {};

	static Game* CreateNewGameOfType(GameStatus type);

	void ReturnAttractModeVictory();
	void ReturnAttractModeFailed();
	void ReturnAttractModeNormal();
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

	void EnterGame();
	void SetCamera();

	void RandomizeShapes();
	
public:

	Camera		m_worldCamera;
	Camera		m_screenCamera;

	std::vector<ShapeEntity2D*> m_shapes;
	

	//static std::vector<Game*> 

	bool		m_isDebug = false;
	bool		m_isPaused = false;
	bool		m_isShaking = false;
	bool		m_isAttractMode = true;
	bool		m_isOver = false;

	float		m_hangingTime = 0.f;
	int			m_current_level = 0;
	float		m_shakingTime = 0.f;

};