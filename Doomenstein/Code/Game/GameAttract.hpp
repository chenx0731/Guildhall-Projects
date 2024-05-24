#pragma once
#include "Game/GameCommon.hpp"
#include "Game/Game.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Core/Vertex_PCU.hpp"

class App;
class Clock;

struct Vec2;

class GameAttract : public Game
{
public:
	GameAttract();
	virtual ~GameAttract();
	virtual void Startup() override;
	virtual void Update() override;
	virtual void Render() const override;
	virtual void Shutdown()override;

	virtual void Enter() override;
	virtual void Exit() override;
	
private:
	void SetCamera();
	void UpdateFromKeyboard();
};