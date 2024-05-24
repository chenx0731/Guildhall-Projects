#define UNUSED(x) (void)(x);
#include "Game/Controller.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Map.hpp"
#include "Game/Actor.hpp"


Controller::Controller(Map* map) : m_map(map)
{
}

Controller::~Controller()
{
}

void Controller::Possess(ActorUID uid)
{
    Actor* preActor = GetActor();
    if (preActor != nullptr)
        preActor->OnUnpossessed();
    m_actorUID = uid;
    Actor* nowActor = GetActor();
    if (nowActor != nullptr)
        nowActor->OnPossessed(this);
}

Actor* Controller::GetActor()
{
 	return m_map->GetActorByUID(m_actorUID);
}



