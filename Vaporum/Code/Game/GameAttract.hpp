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
class Model;
class Map;
class Button;

struct Vec2;


class GameAttract : public Game
{
public:
	GameAttract();
	~GameAttract();
	virtual void Startup() override;
	virtual void Update() override;
	virtual void Render() const override;
	virtual void Shutdown() override;

	virtual void Enter() override;
	virtual void Exit() override;


private:

	void LoadAssets();

	void UpdateFromKeyboard();
	void UpdateFromController();

	void RenderAttractMode() const;
	void RenderUI() const;

	void SetCamera();

	
	
public:
	float		m_nearClipDist = 0.f;
};