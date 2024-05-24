#pragma once
#include "Game/GameCommon.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include <map>

class App;
class Entity;
class Map;
class Player;
class Clock;

struct Vec2;

enum GameStatus 
{
	GAME_STATUS_NULL = -1,

	GAME_STATUS_ATTRACT,
	GAME_STATUS_PLAY,
	GAME_STATUS_PAUSE,
	GAME_STATUS_HANGING,
	GAME_STATUS_DIE,
	GAME_STATUS_WIN,

	GAME_STATUS_NUM
};


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

	//bool Command_Test(EventArgs& args);
private:

	void LoadAssets();
	
	void DeleteGarbageEntities();
	void KillAll();

	void UpdateEntities(float deltaSeconds);
	void UpdateFromKeyboard(float deltaSeconds);
	void UpdateFromController(float deltaSeconds);

	void RenderAttractMode() const;
	void RenderUI() const;
	void RenderEntities() const;
	void RenderPaused() const;
	void RenderTextInBox() const;
	void RenderAnimation() const;
	void DebugRenderEntities() const;

	void EnterGame();
	void SetCamera();
	
public:

	Camera		m_worldCamera;
	Camera		m_screenCamera;

	Vec2		m_screenSize;
	Vec2		m_worldSize;

	GameStatus	m_gameStatus = GAME_STATUS_ATTRACT;

	bool		m_isDebug = false;
	bool		m_isPaused = false;
	bool		m_isShaking = false;
	bool		m_isAttractMode = true;
	bool		m_isOver = false;
	bool		m_isDebugCamera = false;

	float		m_hangingTime = 0.f;
	int			m_current_level = 0;
	float		m_shakingTime = 0.f;
	
	Map*		m_currentMap;
	int			m_currentMapIndex = 0;
	std::string	m_currentMapName;
	Strings		m_mapNames;
	Entity*		m_player;
	Vec2		m_playerRespawnPoint = Vec2(1.5f, 1.5f);

	Clock*		m_clock;
	

	std::map<std::string, Map*> m_maps;
	//std::vector<Map*>	m_maps;
};