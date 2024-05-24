#define UNUSED(x) (void)(x);
#include "Game/Actor.hpp"
#include "Game/Player.hpp"
#include "Game/GameCommon.hpp"
#include "Game/GamePlaying.hpp"
#include "Game/GameMapSelection.hpp"
#include "Game/Controller.hpp"
#include "Game/AI.hpp"
#include "Game/Map.hpp"
#include "Game/Weapon.hpp"
#include "Game/WeaponDefinition.hpp"
#include "Game/ActorDefinition.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Math/FloatRange.hpp"
#include "Engine/Math/Mat44.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Core/DebugRenderSystem.hpp"
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Math/RaycastResult2D.hpp"
#include <map>

Actor::Actor(Map* owner, const char* name, ActorUID uid)
{
	m_map = owner;
	m_actorDef = ActorDefinition::GetActorDef(name);
	m_UID = uid;
	m_health = m_actorDef->m_health;
	GamePlaying* game = (GamePlaying*) g_theGames[(int)GameState::PLAYING];
	m_corpseLifetime = Stopwatch(game->m_clock, m_actorDef->m_corpseLifeTime);
	m_animeClock = Stopwatch(game->m_clock, 0.f);
	m_animeClock.Start();
		new Clock(*game->m_clock);
	// assign ai controller
	
	if (m_actorDef->m_aiEnabled) {
		AI* ai = new AI(owner);
		m_AIController = ai;
	}
	// create weapons
	if (m_actorDef->m_weapons.size() > 0) {
		for (int weaponIndex = 0; weaponIndex < (int)m_actorDef->m_weapons.size(); weaponIndex++) {
			Weapon* weapon = new Weapon(m_actorDef->m_weapons[weaponIndex].c_str(), g_theGames[(int)GameState::PLAYING]->m_clock);
			weapon->m_weaponDef = WeaponDefinition::GetWeaponDef(m_actorDef->m_weapons[weaponIndex]);
			weapon->m_owner = this;
			m_weapons.push_back(weapon);
		}
	}

	if (m_actorDef->m_animGroups.size() > 0) {
		m_nowAnimeName = m_actorDef->m_animGroups[0]->m_name;
		SetNowAnimationByName(m_nowAnimeName);
	}

	if (m_actorDef->m_dieOnSpawn) {
		m_isDead = true;
		m_corpseLifetime.Start();
	}
}

Actor::Actor(const char* name, Vec3 position)
{
	m_actorDef = ActorDefinition::GetActorDef(name);
	m_position = position;

	Game* game = g_theGames[(int)GameState::MAP_SELECT];
	m_corpseLifetime = Stopwatch(game->m_clock, m_actorDef->m_corpseLifeTime);
	m_animeClock = Stopwatch(game->m_clock, 0.f);
 	m_animeClock.Start();
	//new Clock(*game->m_clock);

	if (m_actorDef->m_animGroups.size() > 0) {
		m_nowAnimeName = m_actorDef->m_animGroups[0]->m_name;
		SetNowAnimationByName(m_nowAnimeName);
	}
}

Actor::~Actor()
{
}

void Actor::Update(float deltaSeconds)
{
	if (m_corpseLifetime.HasDurationElapsed()) {
		if (m_actorDef->m_name == "Demon") {
			SpawnInfo spawninfo1;
			spawninfo1.m_actor = "LostSoul";
			Vec3 position = m_position;
				//Vec3(m_position.x, m_position.y, 0);
			position.z += 0.5f;
			spawninfo1.m_position = position;
			//+ (owner->m_actorDef->m_radius + 0.02f) * direction.GetVec2XY().GetNormalized();


			m_map->SpawnActor(spawninfo1);
		}
		m_isGarbage = true;
		return;
	}
	if (m_isDead)
		return;
	
	if (m_controller != nullptr && m_controller == m_AIController && m_health > 0) {
		m_controller->Update(deltaSeconds);
	}
	UpdatePhysics(deltaSeconds);
	RefreshAnimation();
	if (m_weapons.size() > 0) {
		m_weapons[m_currentWeaponIndex]->RefreshAnimation();
	}
	for (auto it = m_actorDef->m_audios.begin(); it != m_actorDef->m_audios.end(); it++) {
		g_theAudio->SetSoundPosition(it->second, m_position);
	}
	std::string text = Stringf("pos: %.1f, %.1f, %.1f  vel: %.1f, %.1f, %.1f", 
		m_position.x, m_position.y, m_position.z,
		m_velocity.x, m_velocity.y, m_velocity.z);
	if (m_actorDef->m_name == "Marine")
		DebugAddMessage(text, 0.f);
}

void Actor::Render(const Camera& camera) const
{
	if (!m_actorDef->m_isVisible)
		return;
	if (m_controller != m_AIController) {
		Player* player = (Player*)m_controller;
		if (player->m_cameraMode == CameraMode::First_Person && camera.m_position == player->m_worldCamera.m_position)
			return;
	}
	std::vector<Vertex_PCUTBN> actorQuads;
	if (m_actorDef->m_rigidbodyType == RigidbodyType::Cylinder) {
		Vec2 originalSize = m_actorDef->m_size;
		Vec3 offset = Vec3(0.f, m_actorDef->m_pivot.x * originalSize.x, m_actorDef->m_pivot.y * originalSize.y);

		//GamePlaying* game = (GamePlaying*)g_theGames[(int)GameState::PLAYING];

		SpriteDefinition def = GetSpriteDefByName(m_nowAnimeName, camera);

		Mat44 billboardMat = GetBillboardMatrix(GetBillboardType(), camera.GetModelMatrix(), m_position);
		if (GetBillboardType() == BillboardType::NONE)
			billboardMat = GetModelMatrix();
		AddVertsForRoundedQuad3D(actorQuads, Vec3(0.f, 0.f, originalSize.y) - offset,
			Vec3(0.f, 0.f, 0.f) - offset, Vec3(0.f, originalSize.x, 0.f) - offset,
			Vec3(0.f, originalSize.x, originalSize.y) - offset, Rgba8::WHITE, def.GetUVs());
		g_theRenderer->SetModelConstants(billboardMat);
		g_theRenderer->BindTexture(&def.GetTexture());
	}
	else if (m_actorDef->m_rigidbodyType == RigidbodyType::AABB3) {
		AABB3 bounds;
		
		bounds.SetDimensions(m_actorDef->m_dimension);
		bounds.SetCenter(Vec3(0.f, 0.f, bounds.GetDimensions().z * 0.5f));
			//= tile.GetBounds();

		float minX = bounds.m_mins.x;
		float minY = bounds.m_mins.y;
		float minZ = bounds.m_mins.z;
		float maxX = bounds.m_maxs.x;
		float maxY = bounds.m_maxs.y;
		float maxZ = bounds.m_maxs.z;

		if (m_actorDef->m_wallSpriteCoords != IntVec2(-1, -1)) {
			AABB2 wallUV = m_actorDef->m_spriteSheet->GetSpriteUVs(m_actorDef->m_wallSpriteCoords);
			// +x
			AddVertsForQuad3D_PNCU(actorQuads, Vec3(maxX, minY, minZ), Vec3(maxX, maxY, minZ), Vec3(maxX, maxY, maxZ), Vec3(maxX, minY, maxZ), Rgba8::WHITE, wallUV);
			// -x
			AddVertsForQuad3D_PNCU(actorQuads, Vec3(minX, maxY, minZ), Vec3(minX, minY, minZ), Vec3(minX, minY, maxZ), Vec3(minX, maxY, maxZ), Rgba8::WHITE, wallUV);
			// +y
			AddVertsForQuad3D_PNCU(actorQuads, Vec3(maxX, maxY, minZ), Vec3(minX, maxY, minZ), Vec3(minX, maxY, maxZ), Vec3(maxX, maxY, maxZ), Rgba8::WHITE, wallUV);
			// -y
			AddVertsForQuad3D_PNCU(actorQuads, Vec3(minX, minY, minZ), Vec3(maxX, minY, minZ), Vec3(maxX, minY, maxZ), Vec3(minX, minY, maxZ), Rgba8::WHITE, wallUV);
		}
		if (m_actorDef->m_ceilingSpriteCoords != IntVec2(-1, -1)) {
			AABB2 ceilingUV = (m_actorDef->m_spriteSheet->GetSpriteUVs(m_actorDef->m_ceilingSpriteCoords));
			// +z
			AddVertsForQuad3D_PNCU(actorQuads, Vec3(maxX, minY, maxZ), Vec3(maxX, maxY, maxZ), Vec3(minX, maxY, maxZ), Vec3(minX, minY, maxZ), Rgba8::WHITE, ceilingUV);
		}
		if (m_actorDef->m_floorSpriteCoords != IntVec2(-1, -1)) {
			AABB2 floorUV = m_actorDef->m_spriteSheet->GetSpriteUVs(m_actorDef->m_floorSpriteCoords);
			// -z
			AddVertsForQuad3D_PNCU(actorQuads, Vec3(maxX, maxY, minZ), Vec3(maxX, minY, minZ), Vec3(minX, minY, minZ), Vec3(minX, maxY, minZ), Rgba8::WHITE, floorUV);
		}
		g_theRenderer->SetLightConstants(Vec3(2, 1, -1), 0.3f, Rgba8::WHITE);
		g_theRenderer->SetModelConstants(GetModelMatrix());
		g_theRenderer->BindTexture(&m_actorDef->m_spriteSheet->GetTexture());
	}



	g_theRenderer->BindShader(m_actorDef->m_shader);
	if (m_actorDef->m_name == "Planet")
		g_theRenderer->SetLightConstants(Vec3(2, 1, -1), 0.3f, Rgba8(100, 100, 200));
	if (m_actorDef->m_name == "Elevator" || m_actorDef->m_name == "PipeBlock" || m_actorDef->m_name == "Skybox")
		g_theRenderer->SetRasterizerMode(RasterizerMode::SOLID_CULL_NONE);

	g_theRenderer->DrawVertexArray((int)actorQuads.size(), actorQuads.data());
	g_theRenderer->BindShader(nullptr);
	g_theRenderer->BindTexture(nullptr);
	g_theRenderer->SetRasterizerMode(RasterizerMode::SOLID_CULL_BACK);
	g_theRenderer->SetModelConstants();

}

Mat44 Actor::GetModelMatrix() const
{
	EulerAngles orientation = m_orientation;
	orientation.m_pitchDegrees = 0.f;
	orientation.m_rollDegrees = 0.f;
	Mat44 mat = orientation.GetMatrix_XFwd_YLeft_ZUp();
	mat.SetTranslation3D(m_position);
	return mat;
}

FloatRange Actor::GetZFloatRange() const
{
	if (m_actorDef->m_rigidbodyType == RigidbodyType::Cylinder)
		return FloatRange(m_position.z, m_position.z + m_actorDef->m_height);

	if (m_actorDef->m_rigidbodyType == RigidbodyType::AABB3)
		return FloatRange(m_position.z, m_position.z + m_actorDef->m_dimension.z);
	return FloatRange::ZERO;
}

Vec3 Actor::GetFwdNormal() const
{
	Vec3 fwdNormal;
	fwdNormal.x = CosDegrees(m_orientation.m_yawDegrees) * CosDegrees(m_orientation.m_pitchDegrees);
	fwdNormal.y = SinDegrees(m_orientation.m_yawDegrees) * CosDegrees(m_orientation.m_pitchDegrees);
	fwdNormal.z = - SinDegrees(m_orientation.m_pitchDegrees);
	return fwdNormal;
}

BillboardType Actor::GetBillboardType() const
{
	if (m_actorDef->m_billBoardType == "WorldUpFacing") {
		return BillboardType::WORLD_UP_CAMERA_FACING;
	}
	if (m_actorDef->m_billBoardType == "WorldUpOpposing") {
		return BillboardType::WORLD_UP_CAMERA_OPPOSING;
	}
	if (m_actorDef->m_billBoardType == "FullOpposing") {
		return BillboardType::FULL_CAMERA_OPPOSING;
	}
	else
	{
		return BillboardType::NONE;
	}
}

bool Actor::IsOnGround()
{
	if (m_isRiding)
		return true;
	Vec3 start = m_position;
	Actor* actor = nullptr;
	RaycastResult3D result = m_map->RaycastAll(start, Vec3(0.f, 0.f, -1.f), 4.f, actor);

	if (result.m_didImpact && result.m_impactDis <= 0.0005f) {
		if (actor) {
			if (actor->m_actorDef->m_name == "JumpBlock") {
				EulerAngles orientation = actor->m_orientation;
				orientation.m_yawDegrees += 45.f;
				Vec3 direction = orientation.GetForwardVector() * (actor->m_position.z * 6.f + 6.f);
				direction.z = actor->m_position.z * 2.f + 2.f;
				AddImpulse(direction * 2.f);
			}
		}
		else {
			float deltaZ = m_position.z - (float)RoundDownToInt(m_position.z);
			if (deltaZ != 0.f)
				m_position.z = (float)RoundDownToInt(m_position.z) + 1.f;
		}
		return true;
	}
		
	return false;
}

SpriteDefinition Actor::GetSpriteDefByName(std::string name, Camera camera) const
{
	//GamePlaying* game = (GamePlaying*)g_theGames[(int)GameState::PLAYING];
	Vec3 view = m_position - camera.m_position;
	if (m_map != nullptr)
		view.z = m_map->m_level;
	for (int animeIndex = 0; animeIndex < m_actorDef->m_animGroups.size(); animeIndex++) {
		if (m_actorDef->m_animGroups[animeIndex]->m_name == name) {
			if (m_actorDef->m_animGroups[animeIndex]->m_scaleBySpeed)
				return m_actorDef->m_animGroups[animeIndex]->GetSpriteDefAtTimeAndDirection(m_animeClock.GetElapsedTime() * m_velocity.GetLength() * 0.2f, m_orientation, view);
			else
				return m_actorDef->m_animGroups[animeIndex]->GetSpriteDefAtTimeAndDirection(m_animeClock.GetElapsedTime(), m_orientation, view);
		}
	}
	//if (m_actorDef->m_animGroups.size() > 0) {
	if (m_actorDef->m_animGroups[0]->m_scaleBySpeed)
		return m_actorDef->m_animGroups[0]->GetSpriteDefAtTimeAndDirection(m_animeClock.GetElapsedTime() * m_velocity.GetLength(), m_orientation, view);
	else
		return m_actorDef->m_animGroups[0]->GetSpriteDefAtTimeAndDirection(m_animeClock.GetElapsedTime(), m_orientation, view);
	//}
}

SpriteAnimGroupDefinition* Actor::GetSpriteAnimGroupDefinitionByState(ActorState myState) const
{
	std::string name;
	if (myState == ActorState::Walk) {
		name = "Walk";
	}
	if (myState == ActorState::Attack) {
		name = "Attack";
	}
	if (myState == ActorState::Hurt) {
		name = "Hurt";
	}
	if (myState == ActorState::Death) {
		name = "Death";
	}
	return GetSpriteAnimGroupDefinitionByName(name);
}

SpriteAnimGroupDefinition* Actor::GetSpriteAnimGroupDefinitionByName(std::string name) const
{
	for (int groupIndex = 0; groupIndex < m_actorDef->m_animGroups.size(); groupIndex++) {
		if (name == m_actorDef->m_animGroups[groupIndex]->m_name) {
			return m_actorDef->m_animGroups[groupIndex];
		}
	}
	return nullptr;
}

void Actor::RefreshAnimation()
{
	if (m_actorDef->m_animGroups.size() <= 0 || m_actorDef->m_name == "Elevator")
		return;
	if (m_animeClock.HasDurationElapsed()) {
		SpriteAnimGroupDefinition* nowGroup = GetSpriteAnimGroupDefinitionByName(m_nowAnimeName);
		if (nowGroup->m_playbackType == SpriteAnimPlaybackType::ONCE) {
			// change to default one
			m_nowAnimeName = m_actorDef->m_animGroups[0]->m_name;
			SetNowAnimationByName(m_nowAnimeName);
		}
	}
}

void Actor::SetNowAnimationByState(ActorState myState)
{
	if (myState == ActorState::Walk) {
		m_nowAnimeName = "Walk";
	}
	if (myState == ActorState::Attack) {
		m_nowAnimeName = "Attack";
	}
	if (myState == ActorState::Hurt) {
		m_nowAnimeName = "Hurt";
	}
	if (myState == ActorState::Death) {
		m_nowAnimeName = "Death";
	}
	SpriteAnimGroupDefinition* temp = GetSpriteAnimGroupDefinitionByState(myState);
	m_animeClock.m_duration = temp->GetDuration();
	m_animeClock.Restart();
}

void Actor::SetNowAnimationByName(std::string name)
{
	SpriteAnimGroupDefinition* temp = GetSpriteAnimGroupDefinitionByName(name);
	if (temp) {
		m_nowAnimeName = name;
		m_animeClock.m_duration = temp->GetDuration();
		m_animeClock.Restart();

		if (m_actorDef->m_audios.find(name) != m_actorDef->m_audios.end())
			PlayMusic(m_actorDef->m_audios.at(name), m_position);
	}
	
}

void Actor::UpdatePhysics(float deltaSeconds)
{
	if (!m_actorDef->m_isSimulated)
		return;
	if (m_isRiding) {
		m_position = m_ridingActor->m_position;
		m_position.z = m_ridingActor->GetZFloatRange().m_max;
	}
	if (!m_actorDef->m_isFlying) {
		//m_position.z = deltaSeconds * -0.1f;
		if(!IsOnGround())
			AddForce(Vec3(0.f, 0.f, - m_map->m_gravity));
	}
	AddForce(- m_actorDef->m_drag * m_velocity.GetVec2XY());
	m_velocity += deltaSeconds * m_acceleration;
	//m_velocity.GetClamped(m_actorDef->m_walkSpeed);
	m_position += deltaSeconds * m_velocity;
	if (m_actorDef->m_name == "LostSoul") {
		GamePlaying* game = (GamePlaying*)g_theGames[(int)GameState::PLAYING];
		m_position.z += 0.05f * SinDegrees(game->m_clock->GetTotalSeconds() * 200.f);
	}
	if (!m_actorDef->m_isFlying) {
		if (IsOnGround() && m_velocity.z < 0.f){
			m_velocity.z = 0.f;
		}
		
	}
	if (m_position.z < -5.f) {
		if (m_actorDef->m_name != "Skybox")
			m_isDead = true;
		//m_position.z = 8.f;
	}
	m_acceleration = Vec3();
}

void Actor::Damage(Actor* actor, int damage)
{
	// set ai target
	if (!m_actorDef->m_canDamage)
		return;
	if (m_isDead)
		return;
	if (m_controller != nullptr && m_controller == m_AIController){
		//&& actor->m_actorDef->m_faction != m_actorDef->m_faction) {
		AI* ai = (AI*)m_controller;
		if (!ai->m_targetUID.IsValid() || m_map->GetActorByUID(ai->m_targetUID) == nullptr) 
			ai->m_targetUID = actor->m_UID;
	}
	m_health -= damage;
	m_isHit = true;
	SetNowAnimationByName("Hurt");
	if (m_health <= 0)
	Die();
}

void Actor::Die()
{
	m_health = 0;
	m_isDead = true;
	m_corpseLifetime.Start();
	if (m_weapons.size() > 0) {
		if (m_weapons[0]->m_weaponDef->m_name == "SoulBomb") {
			m_weapons[0]->Fire(this);
		}
	}
	SetNowAnimationByName("Death");
}

void Actor::AddForce(Vec3 force)
{
	m_acceleration += force;
}

void Actor::AddImpulse(Vec3 velocity)
{
	m_velocity += velocity;
}

void Actor::OnCollide(Actor* actor)
{
	if (m_isDead)
		return;

	if (m_actorDef->m_isDieOnCollide ) {
		m_isDead = true;
		m_corpseLifetime.Start();
		SetNowAnimationByName("Death");
	}
	if (m_actorDef->m_name == "DemonProjectile")
	{
		SpawnInfo spawninfo;
		spawninfo.m_actor = "Demon";
		//Vec3 direction = GetRandomDirectionInCone(owner->m_orientation, m_weaponDef->m_projectileCone);
		
		spawninfo.m_position = m_position - GetFwdNormal() * 0.5f;
		spawninfo.m_orientation = m_orientation;

		SpawnInfo spawninfo1;
		spawninfo1.m_actor = "VacuumHit";
		Vec3 position = m_position - GetFwdNormal() * 0.5f;
			//Vec3(m_position.x, m_position.y, 0);
		position.z += 0.5f;
		spawninfo1.m_position = position;
		//+ (owner->m_actorDef->m_radius + 0.02f) * direction.GetVec2XY().GetNormalized();


		m_map->SpawnActor(spawninfo1);
		Actor* spawned = m_map->SpawnActor(spawninfo);

		m_owner = spawned;
	}
	if (m_actorDef->m_name == "LostSoulProjectile")
	{
		SpawnInfo spawninfo;
		spawninfo.m_actor = "LostSoul";

		spawninfo.m_position = m_position;
			//Vec3(m_position.x, m_position.y, 0);

		//+ (owner->m_actorDef->m_radius + 0.02f) * direction.GetVec2XY().GetNormalized();
		//spawninfo.m_velocity = m_weaponDef->m_projectileSpeed * direction;
		spawninfo.m_orientation = m_orientation;

		SpawnInfo spawninfo1;
		spawninfo1.m_actor = "VacuumHit";
		Vec3 position = m_position;
			//Vec3(m_position.x, m_position.y, 0);
		position.z += 0.5f;
		spawninfo1.m_position = position;
		//+ (owner->m_actorDef->m_radius + 0.02f) * direction.GetVec2XY().GetNormalized();


		m_map->SpawnActor(spawninfo1);
		Actor* spawned = m_map->SpawnActor(spawninfo);

		m_owner = spawned;
	}
	if (actor != nullptr) {
		int damage = g_rng->RollRandomIntInRange((int)m_actorDef->m_damageOnCollide.m_min, (int)m_actorDef->m_damageOnCollide.m_max);
		if (damage == 0)
			return;
		if (m_owner != nullptr) {
			actor->Damage(m_owner, damage);
		}		
		else actor->Damage(this, damage);
		actor->AddImpulse(-actor->m_velocity.GetNormalized() * m_actorDef->m_impulseOnCollide);
	}
}

void Actor::OnPossessed(Controller* controller)
{
	m_controller = controller;
	controller->m_map = m_map;
}

void Actor::OnUnpossessed()
{
	m_controller = m_AIController;
}

void Actor::MoveInDirection(Vec3 direction)
{
	AddForce(direction * m_actorDef->m_drag * m_actorDef->m_runSpeed);
}

void Actor::TurnInDirection(Vec3 direction, float turnDegree)
{
	Vec2 orientation = direction.GetVec2XY();
	float orientationDegree = orientation.GetOrientationDegrees();
	m_orientation.m_yawDegrees = GetTurnedTowardDegrees(m_orientation.m_yawDegrees, orientationDegree, turnDegree);
}

void Actor::Attack(Actor* actor)
{
	UNUSED(actor);
}

void Actor::EquipWeapon()
{
}

