#pragma once
#include "Game/GameCommon.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Core/Vertex_PCU.hpp"

class App;
class Entity;
class PlayerShip;
class Asteroid;
class Bullet;
class Beetle;
class Wasp;
class Debris;
class Clock;
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
	void Update();
	void Render() const;
	void Shutdown();

	Asteroid*	SpawnRandomAsteroid();
	Bullet*		SpawnBullet(Vec2 const& pos, float forwardDegrees);
	Debris*		SpawnRandomDebris(Vec2 startPos, float minRadius, float maxRadius, Rgba8 color, float minSpeed, float maxSpeed);
	Wasp*		SpawnRandomWasp();
	Beetle*		SpawnRandomBeetle();
	void		SpawnNewLevel();

	void ReturnAttractModeVictory();
	void ReturnAttractModeFailed();
	void ReturnAttractModeNormal();
private:
	void CheckBulletsVsAsteroids();
	void CheckBulletVsAsteroid(Bullet& bullet, Asteroid& asteroid);
	
	void CheckBulletsVsBeetles();
	void CheckBulletVsBeetle(Bullet& bullet, Beetle& beetle);

	void CheckBulletsVsWasps();
	void CheckBulletVsWasp(Bullet& bullet, Wasp& wasp);

	void CheckAsteroidsVsShips();
	void CheckAsteroidVsShip(Asteroid& asteroid, PlayerShip& ship);

	void CheckBeetlesVsShips();
	void CheckBeetleVsShip(Beetle& beetle, PlayerShip& ship);

	void CheckWaspsVsShips();
	void CheckWaspVsShip(Wasp& wasp, PlayerShip& ship);

	void DeleteGarbageEntities();
	void KillAll();

	void UpdateEntities();
	void UpdateFromKeyboard();
	void UpdateFromController();

	void RenderAttractMode() const;
	void RenderUI() const;
	void RenderMinimap() const;
	void RenderEntities() const;
	void RenderPaused() const;
	void DebugRenderEntities() const;

	void EnterGame();
	void SetCamera();
	
public:
	PlayerShip* m_playerShip = nullptr;
	Asteroid*	m_asteroids[MAX_ASTEROIDS] = {};
	Bullet*		m_bullets[MAX_BULLETS] = {};
	Debris*		m_debris[MAX_DEBRIS] = {};
	Wasp*		m_wasps[MAX_WASPS] = {};
	Beetle*		m_beetles[MAX_BEETLES] = {};

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

	Clock*		m_clock;

};