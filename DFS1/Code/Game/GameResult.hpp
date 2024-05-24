#pragma once
#include "Game/GameCommon.hpp"
#include "Game/Game.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include <vector>

class App;
class Clock;
class Button;
class Actor;

struct Vec2;

class GameResult : public Game
{
public:
	GameResult();
	virtual ~GameResult();
	virtual void Startup() override;
	virtual void Update() override;
	virtual void Render() const override;
	virtual void Shutdown()override;

	virtual void Enter() override;
	virtual void Exit() override;
	
private:

	void RenderWorld() const;
	void RenderUI() const;

	void SetCamera();
	void UpdateFromKeyboard();
	void EnterLobby();
public:
	std::vector<Button*> m_buttons;
	std::vector<Actor*> m_actors;
	float m_jupiterDegree = 0.f;
	float m_earthDegree = 90.f;

	GameState m_state = GameState::RESULT;
};