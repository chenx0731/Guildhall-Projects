#pragma once
#include "Game/GameCommon.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include <vector>

class App;
class Entity;
class Clock;
class TestJob;

struct Vec2;


class Game
{
public:
	Game( App* owner);
	~Game();
	void Startup();
	void Update();
	void Render() const;
	void Shutdown();

	void ReturnAttractModeVictory();
	void ReturnAttractModeFailed();
	void ReturnAttractModeNormal();
private:

	void LoadAssets();
	
	void DeleteGarbageEntities();
	void KillAll();

	void UpdateEntities();
	void UpdateFromKeyboard();
	void UpdateFromController();

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
	int			m_current_level = 0;
	float		m_shakingTime = 0.f;

	Clock*		m_clock;

	std::vector<TestJob*> m_retrievedJobs;
};