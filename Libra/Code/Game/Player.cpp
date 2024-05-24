#include "Game/Player.hpp"
#include "Game/Map.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Core/NamedStrings.hpp"
#include "Engine/Core/EngineCommon.hpp"

Player::Player(EntityType type, EntityFaction faction, Map* owner, Vec2 const& startPos, float orientationDegrees) : Entity(owner, startPos, orientationDegrees)
{
	m_entityType = type;
	m_faction = faction;
	m_turretOrientation = 0.f;

	m_physicsRadius			= g_gameConfigBlackboard.GetValue("entityPhysicsRadius", 0.3f);
	m_cosmeticRadius		= g_gameConfigBlackboard.GetValue("entityCosmeticRadius", 0.5f);
	m_textureRadius			= g_gameConfigBlackboard.GetValue("entityTextureRadius", 0.5f);
	m_speed					= g_gameConfigBlackboard.GetValue("playerDriveSpeed", 2.f);
	m_angularVelocity		= g_gameConfigBlackboard.GetValue("playerTurnRate", 360.f);
	m_turretAngularVelocity = g_gameConfigBlackboard.GetValue("playerGunTurnRate", 720.f);
	m_health				= g_gameConfigBlackboard.GetValue("playerHealth", 20);
	m_maxHealth				= m_health;

	m_orientationGoal = m_orientationDegrees;
	m_turretOrientationGoal = m_turretOrientationGoal + m_orientationDegrees;
	m_isPushedByWall = true;
	m_isPushedByEntities = true;
	m_doesPushEntities = true;
	m_isHitByBullets = true;
	
}

Player::~Player()
{
}

void Player::Update(float deltaSeconds)
{
	m_fireColdTime += deltaSeconds;
	UpdateFromController(deltaSeconds);
	UpdateFromKeyboard(deltaSeconds);
}

void Player::Render() const
{
	// Tank
	//Texture* tankBaseTexture = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/PlayerTankBase.png");
	std::vector<Vertex_PCU> tankBaseVerts;
	AABB2 localBounds = AABB2(Vec2(-m_textureRadius, -m_textureRadius), Vec2(m_textureRadius, m_textureRadius));
	AddVertsForAABB2D(tankBaseVerts, localBounds, Rgba8(255, 255, 255));
	TransformVertexArrayXY3D((int)tankBaseVerts.size(), tankBaseVerts.data(), 1.0f, m_orientationDegrees, m_position);
	g_theRenderer->BindTexture(g_textureLibrary[TEXTURE_TANKBASE]);
	g_theRenderer->DrawVertexArray((int)tankBaseVerts.size(), tankBaseVerts.data());

	// Turret
	//Texture* tankTopTexture = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/PlayerTankTop.png");
	std::vector<Vertex_PCU> tankTopVerts;
	AddVertsForAABB2D(tankTopVerts, localBounds, Rgba8(255, 255, 255));
	TransformVertexArrayXY3D((int)tankTopVerts.size(), tankTopVerts.data(), 1.0f, m_orientationDegrees + m_turretOrientation, m_position);
	g_theRenderer->BindTexture(g_textureLibrary[TEXTURE_TANKTOP]);
	g_theRenderer->DrawVertexArray((int)tankTopVerts.size(), tankTopVerts.data());

	g_theRenderer->BindTexture(nullptr);

}

void Player::DebugRender() const
{
	float thickness = g_gameConfigBlackboard.GetValue("debugDrawLineThickness", 0.05f);
	Vec2 tankGoalCosmeticRadius = Vec2::MakeFromPolarDegrees(m_orientationGoal, m_cosmeticRadius);
	Vec2 turretGoalCosmeticRadius = Vec2::MakeFromPolarDegrees(m_turretOrientationGoal, m_cosmeticRadius);
	// turret
	DebugDrawLine(m_position, m_orientationDegrees + m_turretOrientation, m_cosmeticRadius, thickness * 3.f, Rgba8(0, 0, 255));
	// turretGoal
	DebugDrawLine(m_position + turretGoalCosmeticRadius, m_turretOrientationGoal, m_cosmeticRadius * 0.3f, thickness * 3.f, Rgba8(0, 0, 255));
	// outer Ring
	DebugDrawRing(m_position, m_cosmeticRadius, thickness, Rgba8(255, 0, 255));
	// inner Ring
	DebugDrawRing(m_position, m_physicsRadius, thickness, Rgba8(0, 255, 255));
	// m_velocity
	DebugDrawLine(m_position, m_position + m_velocity, thickness, Rgba8(255, 255, 0));
	// forward
	DebugDrawLine(m_position, m_orientationDegrees, m_cosmeticRadius, thickness, Rgba8(255, 0, 0));
	// forwardGoal
	DebugDrawLine(m_position + tankGoalCosmeticRadius, m_orientationGoal, m_cosmeticRadius * 0.3f, thickness, Rgba8(255, 0, 0));
	// relative-left
	DebugDrawLine(m_position, m_orientationDegrees + 90.f, m_cosmeticRadius, thickness, Rgba8(0, 255, 0));
}

void Player::FireBullet()
{
	float bulletOrientation = m_orientationDegrees + m_turretOrientation;
	Entity* bullet = m_map->SpawnNewEntity(ENTITY_TYPE_GOOD_BOLT, m_position + m_physicsRadius * Vec2::MakeFromPolarDegrees(bulletOrientation), bulletOrientation);
	m_map->AddEntityToMap(bullet);
	PlayMusic(MUSIC_PLAYER_SHOOT);
}

void Player::Die()
{
	PlayMusic(MUSIC_PLAYER_DIE);
	Entity::Die();
	Entity* explosion = m_map->SpawnNewEntity(ENTITY_TYPE_GOOD_EXPLOSION, m_position, 0.f);
	m_map->AddEntityToMap(explosion);
}

void Player::ReactToBulletImpact(Entity* bullet)
{
	PlayMusic(MUSIC_PLAYER_HIT);
	Entity::ReactToBulletImpact(bullet);
}


void Player::UpdateFromKeyboard(float deltaSeconds)
{
	Vec2 tankDirection = Vec2(0.f, 0.f);
	Vec2 turretDirection = Vec2(0.f, 0.f);

	if (g_theInput->IsKeyDown(' ')) {
		if (m_fireColdTime >= 0.1f) {
			FireBullet();
			m_fireColdTime = 0.f;
		}
	}

	if (g_theInput->IsKeyDown('E')) {
		tankDirection += Vec2(0.f, 1.f);
	}

	if (g_theInput->IsKeyDown('D')) {
		tankDirection += Vec2(0.f, -1.f);
	}

	if (g_theInput->IsKeyDown('S')) {
		tankDirection += Vec2(-1.f, 0.f);
	}

	if (g_theInput->IsKeyDown('F')) {
		tankDirection += Vec2(1.f, 0.f);
	}

	if (g_theInput->IsKeyDown('I')) {
		turretDirection += Vec2(0.f, 1.f);
	}

	if (g_theInput->IsKeyDown('K')) {
		turretDirection += Vec2(0.f, -1.f);
	}

	if (g_theInput->IsKeyDown('J')) {
		turretDirection += Vec2(-1.f, 0.f);
	}

	if (g_theInput->IsKeyDown('L')) {
		turretDirection += Vec2(1.f, 0.f);
	}

	if (tankDirection.GetLengthSquared() > 0.f)
	{
		tankDirection.Normalize();
		float goalAngular = Atan2Degrees(tankDirection.y, tankDirection.x);
		m_orientationDegrees = GetTurnedTowardDegrees(m_orientationDegrees, goalAngular, m_angularVelocity * deltaSeconds);
		tankDirection = Vec2(CosDegrees(m_orientationDegrees), SinDegrees(m_orientationDegrees));
		m_orientationGoal = goalAngular;
	}
	//if (!m_isDead) {
		m_velocity = tankDirection * m_speed;
		m_position += m_velocity * deltaSeconds;
	//}

	if (turretDirection.GetLengthSquared() > 0.f)
	{
		//if (!m_isDead) {
			float goalAngular = Atan2Degrees(turretDirection.y, turretDirection.x);
			m_turretOrientation = GetTurnedTowardDegrees(m_turretOrientation, goalAngular - m_orientationDegrees, m_turretAngularVelocity * deltaSeconds);
			m_turretOrientationGoal = goalAngular;
		//}
	}

}

void Player::UpdateFromController(float deltaSeconds)
{
	XboxController tempController = g_theInput->GetController(0);
	Vec2 tankDirection = Vec2(0.f, 0.f);

	if (tempController.GetLeftStick().GetMagnitude() > 0.f) {
		float degree = tempController.GetLeftStick().GetOrientationDegrees();
		tankDirection = Vec2(CosDegrees(degree), SinDegrees(degree));
		tankDirection *= tempController.GetLeftStick().GetMagnitude();
		//if (!m_isDead) {
			m_orientationDegrees = GetTurnedTowardDegrees(m_orientationDegrees, degree, m_angularVelocity * deltaSeconds);
			m_orientationGoal = degree;
			m_velocity = tankDirection * m_speed;
			m_position += m_velocity * deltaSeconds;
		//}
	}

	if (tempController.GetRightStick().GetMagnitude() > 0.f) {
		float degree = tempController.GetRightStick().GetOrientationDegrees();
		//if (!m_isDead) {
			m_turretOrientation = GetTurnedTowardDegrees(m_turretOrientation, degree - m_orientationDegrees, m_turretAngularVelocity * deltaSeconds);
			m_turretOrientationGoal = degree;
		//}
	}

	if (tempController.GetRightTrigger() > 0.f) {
		if (m_fireColdTime >= 0.1f) {
			FireBullet();
			m_fireColdTime = 0.f;
		}
	}

}
