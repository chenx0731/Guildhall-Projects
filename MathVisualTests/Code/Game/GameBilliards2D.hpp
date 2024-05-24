#pragma once
#include "Game/GameCommon.hpp"
#include "Game/ShapeEntity2D.hpp"
#include "Game/Game.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Core/Vertex_PCU.hpp"


class App;
class Entity;

struct Vec2;

class GameBilliards2D : public Game
{
public:
	GameBilliards2D();
	virtual ~GameBilliards2D();
	virtual void Startup() override;
	virtual void Update(float deltaSeconds) override;
	virtual void Render() const override;
	virtual void Shutdown() override;

	void ReturnAttractModeVictory();
	void ReturnAttractModeFailed();
	void ReturnAttractModeNormal();
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

	bool CheckIfOffScreen(Vec2& position, float radius, Vec2& velocity);
	
public:
	Vec2 m_cursor;
	Vec2 m_arrowStart = Vec2(0.f, 0.f);
	Vec2 m_arrowEnd = Vec2(0.f, 0.f);
	bool m_didImpact = false;
	Vec2 m_impactPos = Vec2(0.f, 0.f);
	Vec2 m_impactNormal = Vec2(0.f, 0.f);

	std::vector<ShapeEntity2D*> m_billiards;

	bool m_setStart = false;
	bool m_setEnd = false;
};