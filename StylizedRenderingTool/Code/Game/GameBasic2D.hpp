#pragma once
#include "Game/GameCommon.hpp"
#include "Game/Game.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Core/Vertex_PCU.hpp"

class App;
class Entity;
class Player;
class Prop;
class Clock;
class Shader;
class Texture;
class ConstantBuffer;

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
	void SetConstantBufferForShader();

	void RenderAttractMode() const;
	void RenderUI() const;
	void RenderEntities() const;
	void RenderPaused() const;
	void DebugRenderEntities() const;
	void RenderGrid() const;
	void RenderFilter() const;

	void EnterGame();
	void SetCamera();
	
public:

	Camera		m_worldCamera;
	Camera		m_screenCamera;

	bool		m_isDebug = false;
	bool		m_isPaused = false;
	bool		m_isShaking = false;
	bool		m_isAttractMode = false;
	bool		m_isOver = false;

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

};