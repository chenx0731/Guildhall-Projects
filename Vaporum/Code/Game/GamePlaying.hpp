#pragma once
#include "Game/GameCommon.hpp"
#include "Game/Game.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Math/IntVec2.hpp"
#include <string>

class App;
class Entity;
class Player;
class Prop;
class Clock;
class Model;
class Map;
class Unit;
class Tile;

struct Vec2;


class GamePlaying : public Game
{
public:
	GamePlaying();
	~GamePlaying();
	virtual void Startup() override;
	virtual void Update() override;
	virtual void Render() const override;
	virtual void Shutdown() override;

	virtual void Exit() override;
	virtual void Enter() override;

	Vec3 CalcuteMouseRayDirection() const;

	IntVec2 GetHoveringCoords() const;
	Vec3 GetHoveringPosition() const;
	Tile* GetHoveringTile() const;

	void StartTurn();
	void EndTurn();
	void ConfirmEndTurn();


	void Select();
	void SelectLeft();
	void SelectRight();
	void Cancel();
	void Move();
	void ConfirmMove();
	void Attack();
	void ConfirmAttack();
	void SetCurUnit(IntVec2 currentCoords);
	void SetTarget(IntVec2 targetCoords);

	void Send(std::string msg);

	int DoesAnyPlayerWin() const;

	Player* GetLocalPlayer() const;
	Player* GetRemotePlayer() const;
private:

	void LoadAssets();
	
	void DeleteGarbageEntities();
	void KillAll();

	void UpdateButtonTexts();
	void UpdateEntities();
	void UpdateFromKeyboard();
	void UpdateTargetHighlight();
	void UpdateFromController();

	void RenderAttractMode() const;
	void RenderUI() const;
	void RenderPrompt() const;
	void RenderNetPrompt() const;
	void RenderEntities() const;
	void RenderPaused() const;
	void DebugRenderEntities() const;
	void RenderGrid() const;
	void RenderSelectedUnit() const;
	

	void EnterGame();
	void SetCamera();

	Unit* GetUnitByPosition(const Vec3& position) const;
	Tile* GetTileByPosition(const Vec3& position) const;

	Unit* GetHoveringUnit() const;

	Player* GetWaitingPlayer() const;

	bool IsCurrentPlayerLocal() const;

	bool IsConnected() const;

	bool AreBothReady() const;

public:

	bool		m_isDebug = false;
	bool		m_isPaused = false;
	bool		m_isShaking = false;
	bool		m_isAttractMode = true;
	bool		m_isOver = false;

	bool		m_isRemoteReady = false;

	float		m_hangingTime = 0.f;
	int			m_current_level = 0;
	float		m_shakingTime = 0.f;
	float		m_nearClipDist = 0.f;

	Player*		m_currentPlayer;
	int			m_currentPlayerIndex = 0;

	IntVec2		m_targetCoords;
	Vec3		m_hoveringPos = Vec3();
	Unit*		m_selectedUnit = nullptr;
	Unit*		m_attackedUnit = nullptr;
	Tile*		m_selectedTile = nullptr;
	IntVec2		m_selectCoords;
	IntVec2		m_OGCoords;
	std::vector<Player*> m_players;
	Prop*		m_prop1;
	Prop*		m_prop2;
	Prop*		m_sphere;
	Model*		m_model = nullptr;
	Map*		m_map = nullptr;

};