#pragma once
#include "Game/Controller.hpp"

class AI : public Controller
{
public:
	AI(Map* map);
	~AI();
	void DamageBy(ActorUID uid);
	void Update(float deltaSeconds) override;
public:
	ActorUID m_targetUID;
};