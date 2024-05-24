#pragma once
#include "Game/GameCommon.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Core/Vertex_PCU.hpp"

class App;
class Entity;

struct Vec2;

struct Level
{
	int asteroid_num = 0;
	int beetle_num = 0;
	int wasp_num = 0;
	int total_num = 0;
	int current_kill = 0;
};

const int NUM_LEVEL = 5;

class Game
{
public:
	Game( App* owner);
	~Game();
	void Startup();
	void Update(float deltaSeconds);
	void Render() const;
	void Shutdown();

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
	
public:

	Camera		m_worldCamera;
	Camera		m_screenCamera;

	bool		m_isDebug = false;
	bool		m_isPaused = false;
	bool		m_isShaking = false;
	bool		m_isAttractMode = true;
	bool		m_isOver = false;

	float		m_hangingTime = 0.f;
	Level		m_levels[NUM_LEVEL];
	int			m_current_level = 0;
	float		m_shakingTime = 0.f;

	Vec2		m_starsPosition[MAX_STARS];
	float		m_starsRadius[MAX_STARS];

};