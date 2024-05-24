#pragma once
#include "Game/GameCommon.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Core/Vertex_PCU.hpp"

class App;
class Entity;
class Player;
class Prop;
class Clock;
class Shader;

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
	void RenderGrid() const;

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

	Player*		m_player;
	Prop*		m_prop1;
	Prop*		m_prop2;
	Prop*		m_sphere;
	Shader*		m_shader = nullptr;

	Clock*		m_clock;
};