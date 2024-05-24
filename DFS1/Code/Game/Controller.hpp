#pragma once
#include "Game/ActorUID.hpp"

class Map;
class Actor;

enum class ControllerType
{
	KEYBOARD,
	CONTROLLER0,
	CONTROLLER1,
};

class Controller
{
public:
	Controller(Map* map);
	~Controller();
	void Possess(ActorUID uid);
	Actor* GetActor();
	virtual void Update(float deltaSeconds) = 0;

public:
	ActorUID m_actorUID;
	Map* m_map = nullptr;
};