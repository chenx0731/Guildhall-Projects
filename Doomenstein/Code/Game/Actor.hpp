#pragma once
#include "Game/ActorUID.hpp"
#include "Engine/Core/Stopwatch.hpp"
#include "Engine/Math/EulerAngles.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include <vector>
#include <string>

class Map;
class Weapon;
class Camera;
class Controller;
struct FloatRange;
class ActorDefinition;
class SpriteDefinition;
class SpriteAnimGroupDefinition;
enum class BillboardType;

enum class ActorState
{
	Walk,
	Hurt,
	Death,
	Attack,
};

class Actor
{
public:
	Actor(Map* owner, const char* name, ActorUID uid);
	~Actor();
	void Update(float deltaSeconds);
	void Render(const Camera& camera) const;
	Mat44 GetModelMatrix() const;

	FloatRange GetZFloatRange() const;
	Vec3 GetFwdNormal() const;
	BillboardType GetBillboardType() const;

	SpriteDefinition GetSpriteDefByName(std::string name, Camera camera) const;
	SpriteAnimGroupDefinition* GetSpriteAnimGroupDefinitionByState(ActorState state) const;
	SpriteAnimGroupDefinition* GetSpriteAnimGroupDefinitionByName(std::string name) const;
	void RefreshAnimation();

	void SetNowAnimationByState(ActorState myState);
	void SetNowAnimationByName(std::string name);

	void UpdatePhysics(float deltaSeconds);
	void Damage(Actor* actor, int damage);
	void Die();
	void AddForce(Vec3 force);
	void AddImpulse(Vec3 velocity);

	void OnCollide(Actor* actor);
	void OnPossessed(Controller* controller);
	void OnUnpossessed();

	void MoveInDirection(Vec3 directio);
	void TurnInDirection(Vec3 direction, float turnDegree);

	void Attack(Actor* actor);
	void EquipWeapon();
public:
	ActorUID m_UID;
	ActorDefinition const* m_actorDef = nullptr;
	Map* m_map;
	Vec3 m_position;
	EulerAngles m_orientation;
	Vec3 m_velocity;
	Vec3 m_acceleration;
	Stopwatch m_corpseLifetime;
	bool m_isHit;

	ActorState m_state;

	std::string m_nowAnimeName;
	Stopwatch m_animeClock;
	std::vector<Vertex_PCU> m_vertexes;
	std::vector<Weapon*> m_weapons;

	int m_currentWeaponIndex = 0;

	Actor* m_owner;

	bool m_isDead = false;
	bool m_isGarbage = false;
	int m_health;

	Controller* m_controller;

	Controller* m_AIController;
};