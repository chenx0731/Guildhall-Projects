#pragma once
#include "Game/WeaponDefinition.hpp"
#include "Engine/Math/EulerAngles.hpp"
#include "Engine/Core/Stopwatch.hpp"

class Actor;
class Camera;

class Weapon
{
public:
	Weapon(const char* name, const Clock* clock);
	void Fire(Actor* owner);
	Vec3 GetRandomDirectionInCone(EulerAngles direction, float degree);
	void RenderHUD(const Camera& camera);

	SpriteDefinition GetSpriteDefByName(std::string name) const;
	SpriteAnimGroupDefinition GetSpriteAnimGroupDefinitionByName(std::string name) const;
	void RefreshAnimation();

	void SetNowAnimationByName(std::string name);
public:
	WeaponDefinition const* m_weaponDef;
	Stopwatch m_refireTime;
	Actor*	  m_owner;
	std::string m_nowAnimeName;
	Stopwatch m_animeClock;

	bool m_stateToggle = 0;
	std::string		m_vacuumProjectile = "";
};
