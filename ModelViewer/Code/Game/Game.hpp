#pragma once
#include "Game/GameCommon.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Math/EulerAngles.hpp"

class App;
class Entity;
class Player;
class Prop;
class Clock;
class Model;

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

	bool		m_isDebug = true;
	bool		m_isPaused = false;
	bool		m_isShaking = false;
	bool		m_isAttractMode = true;
	bool		m_isOver = false;

	float		m_hangingTime = 0.f;
	int			m_current_level = 0;
	float		m_shakingTime = 0.f;

	Player*		m_player;
	Prop*		m_cube;
	Prop*		m_prop2;
	Prop*		m_sphere;
	Model*		m_model = nullptr;
	
	EulerAngles	m_sunOrientation;
	Vec3		m_sunDirection = Vec3(0.f, 0.f, -1.f);
	float		m_sunIntensity = 0.9f;
	float		m_ambientIntensity = 0.1f;
	int			m_normalMode = 0;
	int			m_specularMode = 0;
	float		m_specularIntensity = 1.f;
	float		m_specularPower = 32.f;

	Clock*		m_clock;
};