#pragma once
#include "Game/GameCommon.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include <vector>

class App;
class Clock;
class Button;
struct Vec2;

enum class GameState
{
	NONE,
	ATTRACT,
	MAIN,
	PAUSE,
	PLAYING,

	COUNT
};

class Game
{
public:
	Game();
	virtual ~Game();
	virtual void Startup() {};
	virtual void Update() = 0;
	virtual void Render() const = 0;
	virtual void Shutdown();

	virtual void Enter() = 0;
	virtual void Exit() = 0;

	static Game* CreateOrGetGameStateOfType(GameState state);

public:

	Camera		m_worldCamera;
	Camera		m_screenCamera;
	std::vector<Button*> m_buttons;
	Button*		m_focusedButton = nullptr;
	int			m_focusedIndex = 0;
	Clock* m_clock;
};