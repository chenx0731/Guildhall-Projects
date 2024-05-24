#pragma once
#include "Game/GameCommon.hpp"
#include "Game/Game.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Math/CubicBezierCurve2D.hpp"
#include <map>
#include <string>

class App;
class Clock;
class Map;
class Player;
class World;
enum class ControllerType;
struct Vec2;

class GamePlaying : public Game
{
public:
	GamePlaying();
	virtual ~GamePlaying();
	virtual void Startup() override;
	virtual void Update() override;
	virtual void Render() const override;
	virtual void Shutdown()override;

	virtual void Enter() override;
	virtual void Exit() override;

	void AddPlayerWithSpecController(ControllerType type, int playerIndex);
	void DeletePlayerWithSpecController(ControllerType type);
	void RemovePlayerFromMap(int playerIndex);
	void AddPlayerToMap(int playerIndex, Map* map);
	void SetCameraByPlayerNum();

private: 
	void LoadAssets();
	void SetCamera();
	void UpdateFromKeyboard();
	void RenderScreen() const;
	void RenderWorld() const;
	void RenderUI() const;

public:
	GameState m_state = GameState::PLAYING;
	Map*							m_currentMap;
	std::string						m_currentMapName;
	Strings							m_mapNames;

	std::string						m_currentWorldName;
	Strings							m_worldNames;
	World*							m_currentWorld;

	Player*							m_players[2];
	float							m_elevatorGoal = 0.5f;
	int								m_playerDeath[2] = {0};
	int								m_playerNum = 0;
	std::map<std::string, Map*>		m_maps;
	//std::map<std::string, World*>	m_worlds;
};