#pragma once
#include "Game/GameCommon.hpp"
#include "Game/ShapeEntity2D.hpp"
#include "Game/Game.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Core/Vertex_PCU.hpp"


class App;
class Entity;

struct Vec2;


const int NUM_LEVEL = 5;

class GameNearestPoint : public Game
{
public:
	GameNearestPoint();
	virtual ~GameNearestPoint();
	virtual void Startup() override;
	virtual void Update(float deltaSeconds) override;
	virtual void Render() const override;
	virtual void Shutdown() override;
private:

	void DeleteGarbageEntities();
	void KillAll();

	void UpdateEntities(float deltaSeconds);
	void UpdateFromKeyboard(float deltaSeconds);
	void UpdateFromController(float deltaSeconds);

	void RenderAttractMode() const;
	void RenderUI() const;
	void RenderEntities() const;
	void RenderPaused() const;
	void DebugRenderEntities() const;

	void EnterGame();
	void SetCamera();

	void RandomizeShapes();
	
public:
	Vec2		m_cursor;

};