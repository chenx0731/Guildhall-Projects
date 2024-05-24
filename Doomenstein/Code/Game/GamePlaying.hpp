#pragma once
#include "Game/GameCommon.hpp"
#include "Game/Game.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Core/StringUtils.hpp"
#include <map>
#include <string>

class App;
class Clock;
class Map;
class Player;
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

	Map* GetMapByLevel(float level);

private: 
	void LoadAssets();
	void SetCamera();
	void UpdateFromKeyboard();
	void RenderScreen() const;
	void RenderWorld() const;
	void RenderUI() const;

public:
	Map*						m_currentMap;
	int							m_currentMapIndex = 0;
	std::string					m_currentMapName;
	Strings						m_mapNames;
	Player*						m_players[2];
	float						m_elevatorGoal = 0.5f;
	int							m_playerDeath[2] = {0};
	int							m_playerNum = 0;
	std::map<std::string, Map*> m_maps;
};