#pragma once
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Core/Vertex_PCU.hpp"

class App;
class Entity;
class Player;
class Prop;
class Clock;
class Shader;

struct Vec2;

enum class GameStatus
{
	NONE = -1,

	BASIC_2D,
	BASIC_3D,

	NUM,
};

class Game
{
public:
	Game();
	virtual ~Game();
	virtual void Startup();
	virtual void Update() = 0;
	virtual void Render() const = 0;
	virtual void Shutdown() {};

	virtual void Enter() = 0;
	virtual void Exit() = 0;
	
	static Game* CreateOrGetNewGameOfType(GameStatus type);
private:

	void LoadAssets();

	void EnterGame();
	void SetCamera();
	
public:
	GameStatus	m_status;

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