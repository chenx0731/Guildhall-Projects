#pragma once
#include "Game/GameCommon.hpp"
#include "Game/Game.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Core/Vertex_PNCU.hpp"
#include "Engine/Renderer/SpriteAnimDefinition.hpp"
#include "Engine/Math/CubicBezierCurve2D.hpp"

class App;
class Clock;
class GamePlaying;
class Actor;
class VertexBuffer;
class IndexBuffer;
class Shader;

struct Vec2;

class GameMapSelection : public Game
{
public:
	GameMapSelection();
	virtual ~GameMapSelection();
	virtual void Startup() override;
	virtual void Update() override;
	virtual void Render() const override;
	virtual void Shutdown()override;

	virtual void Enter() override;
	virtual void Exit() override;
	
private:
	void SetCamera();
	void UpdateFromKeyboard();
	float GetProcessedTime() const;

	Mat44 GetShipModelMatrix() const;

	void RenderUI() const;
	void RenderWorld() const;
	void RenderArrows() const;

	Actor* ClosestPlanet() const;
public:
	GamePlaying* m_gamePlaying;
	GameState m_state = GameState::LOBBY;
	CubicHermiteSpline m_splines[10];
	int					m_nowSplineIndex = 0;
	SpriteAnimDefinition* m_planet1 = nullptr;
	std::vector<Actor*> m_actors;
	float m_shipOrientation = 0.f;
	bool m_isDirtPositive = false;
	float m_time;
	Vec2 m_shipPosition;

	std::vector<Vertex_PCUTBN>	m_objVerts;
	std::vector<unsigned int>	m_objIndexes;

	VertexBuffer* m_vertexBuffer;
	IndexBuffer* m_indexBuffer;

	Shader* m_shader;

};