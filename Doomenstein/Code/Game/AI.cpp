#define UNUSED(x) (void)(x);
#include "Game/AI.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Map.hpp"
#include "Game/Actor.hpp"
#include "Game/ActorDefinition.hpp"
#include "Game/Weapon.hpp"
#include "Game/WeaponDefinition.hpp"
#include "Engine/Math/MathUtils.hpp"

AI::AI(Map* map) : Controller(map)
{

}

AI::~AI()
{
}

void AI::DamageBy(ActorUID uid)
{
	m_targetUID = uid;
}

void AI::Update(float deltaSeconds)
{
	Actor* myActor = GetActor();
	if (!m_targetUID.IsValid() || m_map->GetActorByUID(m_targetUID) == nullptr) {
		Actor* actor = m_map->GetClosetVisibleEnemy(myActor);
		if (actor != nullptr)
			m_targetUID = actor->m_UID;
	}
	Actor* myTarget = m_map->GetActorByUID(m_targetUID);
	if (myActor != nullptr && myTarget != nullptr) {
		myActor->TurnInDirection(myTarget->m_position - myActor->m_position, myActor->m_actorDef->m_turnSpeed * deltaSeconds);
		Vec3 iNormal, jNormal, kNormal;
		myActor->m_orientation.GetVectors_XFwd_YLeft_ZUp(iNormal, jNormal, kNormal);
		myActor->MoveInDirection(iNormal);

		// within the melee range, attack the target
		if (myActor->m_weapons.size() > 0) {
			if (myActor->m_weapons[0]->m_weaponDef->m_name == "DemonMelee") {
				if (GetDistance3D(myActor->m_position, myTarget->m_position) < myActor->m_weapons[0]->m_weaponDef->m_meleeRange) {
					//myActor->Attack(myTarget);
					myActor->m_weapons[0]->Fire(myActor);
				}
			}
			if (myActor->m_weapons[0]->m_weaponDef->m_name == "SoulBomb") {
				if (GetDistance3D(myActor->m_position, myTarget->m_position) < myActor->m_weapons[0]->m_weaponDef->m_meleeRange) {
					//myActor->Attack(myTarget);
					//myActor->m_weapons[0]->Fire(myActor);
					myActor->Die();
				}
			}
		}
	}

}


