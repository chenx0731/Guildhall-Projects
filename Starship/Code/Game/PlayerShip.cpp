#define UNUSED(x) (void)(x);
#include "Game/PlayerShip.hpp"
#include "Game/App.hpp"
#include "Game/Game.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Math/MathUtils.hpp"

PlayerShip::PlayerShip(Game* owner, Vec2 const& startPos)
	:Entity(owner, startPos)
{
	m_physicsRadius = PLAYER_SHIP_PHYSICE_RADIUS;
	m_cosmeticRadius = PLAYER_SHIP_COSMETIC_RADIUS;
	m_angularVelocity = PLAYER_SHIP_TURN_SPEED;
	m_health = PLAYER_SHIP_HEALTH;
	InitializeLocalVerts();
}

PlayerShip::~PlayerShip()
{
}

void PlayerShip::Update()
{
	UpdateFromKeyboard();
	UpdateFromController();
	BounceOffWalls();
	Vec2 accelerate = (m_thrustFractionKeyboard + m_thrustFractionController) * PLAYER_SHIP_ACCELERATION * Vec2(CosDegrees(m_orientationDegrees), SinDegrees(m_orientationDegrees));
	m_velocity = m_velocity + accelerate;
	if (!m_isDead)
	m_position += (m_velocity * m_game->m_clock->GetDeltaSeconds());
	if (m_health == 0 && m_isDead)
	{
		m_hangingTime += m_game->m_clock->GetDeltaSeconds();
	}
	if (m_hangingTime >= 3.f)
	{
		m_game->ReturnAttractModeFailed();
		return;
	}
	m_vibrationTime += m_game->m_clock->GetDeltaSeconds();
	if (m_vibrationTime >= PLAYER_SHIP_FIRE_VIBRATION)
	{
		g_theInput->GetController(0).SetVibration(0, 0);
		m_vibrationTime = 0.f;
	}
}

void PlayerShip::Render() const
{
	Vertex_PCU tempShipWorldVerts[NUM_SHIP_VERTS];

	for (int vertIndex = 0; vertIndex < NUM_SHIP_VERTS; vertIndex++)
	{
		tempShipWorldVerts[vertIndex] = m_localVerts[vertIndex];
	}

	if (m_isDead)
	{
		return;
	}
	if (m_isThrustingKeyboard || m_isThrustingController)
	{
		TransformVertexArrayXY3D(NUM_SHIP_VERTS, tempShipWorldVerts, 1.f, m_orientationDegrees, m_position);
		g_theRenderer->DrawVertexArray(NUM_SHIP_VERTS, tempShipWorldVerts);
	}
	else
	{
		TransformVertexArrayXY3D(NUM_SHIP_VERTS - 3, tempShipWorldVerts, 1.f, m_orientationDegrees, m_position);
		g_theRenderer->DrawVertexArray(NUM_SHIP_VERTS - 3, tempShipWorldVerts);
	}
	
}

void PlayerShip::Die()
{
	SoundID testSound = g_theAudio->CreateOrGetSound("Data/Audio/PlayerDied.wav");
	g_theAudio->StartSound(testSound);

	m_isDead = true;
	m_velocity = Vec2(0.f, 0.f);
	m_game->m_isShaking = true;
	int debrisNum = g_rng->RollRandomIntInRange(DEBRIS_PLAYER_DEATH_MIN, DEBRIS_PLAYER_DEATH_MAX);
	for (int debrisIndex = 0; debrisIndex < debrisNum; debrisIndex++)
	{
		m_game->SpawnRandomDebris(m_position, 0.2f, 1.5f, m_color, 5.f, 15.f);
	}
}

void const PlayerShip::CopyPlayershipVerts(Vertex_PCU* original)
{
	original[0].m_position = Vec3(1.f, 0.f, 0.f);
	original[1].m_position = Vec3(0.f, 1.f, 0.f);
	original[2].m_position = Vec3(0.f, -1.f, 0.f);

	// Left wing
	original[3].m_position = Vec3(2.f, 1.f, 0.f);
	original[4].m_position = Vec3(0.f, 2.f, 0.f);
	original[5].m_position = Vec3(-2.f, 1.f, 0.f);

	// Right wing
	original[6].m_position = Vec3(2.f, -1.f, 0.f);
	original[7].m_position = Vec3(-2.f, -1.f, 0.f);
	original[8].m_position = Vec3(0.f, -2.f, 0.f);

	// Body (quad tri 1 / 2)
	original[9].m_position = Vec3(0.f, 1.f, 0.f);
	original[10].m_position = Vec3(-2.f, -1.f, 0.f);
	original[11].m_position = Vec3(0.f, -1.f, 0.f);

	// Body (quad tri 2 / 2)
	original[12].m_position = Vec3(0.f, 1.f, 0.f);
	original[13].m_position = Vec3(-2.f, 1.f, 0.f);
	original[14].m_position = Vec3(-2.f, -1.f, 0.f);

	for (int vertIndex = 0; vertIndex < NUM_SHIP_VERTS - 3; vertIndex++)
	{
		original[vertIndex].m_color = Rgba8(102, 153, 204);
	}
}

void PlayerShip::InitializeLocalVerts()
{
	// Nose cone
	m_localVerts[0].m_position = Vec3(1.f, 0.f, 0.f);
	m_localVerts[1].m_position = Vec3(0.f, 1.f, 0.f);
	m_localVerts[2].m_position = Vec3(0.f, -1.f, 0.f);

	// Left wing
	m_localVerts[3].m_position = Vec3(2.f, 1.f, 0.f);
	m_localVerts[4].m_position = Vec3(0.f, 2.f, 0.f);
	m_localVerts[5].m_position = Vec3(-2.f, 1.f, 0.f);
	
	// Right wing
	m_localVerts[6].m_position = Vec3(2.f, -1.f, 0.f);
	m_localVerts[7].m_position = Vec3(-2.f, -1.f, 0.f);
	m_localVerts[8].m_position = Vec3(0.f, -2.f, 0.f);

	// Body (quad tri 1 / 2)
	m_localVerts[9].m_position = Vec3(0.f, 1.f, 0.f);
	m_localVerts[10].m_position = Vec3(-2.f, -1.f, 0.f);
	m_localVerts[11].m_position = Vec3(0.f, -1.f, 0.f);

	// Body (quad tri 2 / 2)
	m_localVerts[12].m_position = Vec3(0.f, 1.f, 0.f);
	m_localVerts[13].m_position = Vec3(-2.f, 1.f, 0.f);
	m_localVerts[14].m_position = Vec3(-2.f, -1.f, 0.f);

	// Flame
	m_localVerts[15].m_position = Vec3(-2.f, 0.7f, 0.f);
	m_localVerts[16].m_position = Vec3(-2.f, -0.7f, 0.f);
	m_localVerts[17].m_position = Vec3(-5.f, 0.f, 0.f);

	for (int vertIndex = 0; vertIndex < NUM_SHIP_VERTS - 3; vertIndex++)
	{
		m_localVerts[vertIndex].m_color = m_color;
	}

	m_localVerts[15].m_color = Rgba8(255, 0, 0);
	m_localVerts[16].m_color = Rgba8(255, 0, 0);
	m_localVerts[17].m_color = Rgba8(255, 255, 255, 0);
}

void PlayerShip::UpdateFromKeyboard()
{
	/*if (m_isDead)
	{
		return;
	}*/

	if (g_theInput->WasKeyJustPressed(' '))
	{
		Vec2 bulletPos = Vec2(m_position.x + CosDegrees(m_orientationDegrees), m_position.y + SinDegrees(m_orientationDegrees));
		m_game->SpawnBullet(bulletPos, m_orientationDegrees);
	}
	if (g_theInput->WasKeyJustPressed('I'))
	{
		m_game->SpawnRandomAsteroid();
	}
	if (g_theInput->IsKeyDown('S'))
	{
		//m_orientationDegrees = m_orientationDegrees + m_angularVelocity;
		m_orientationDegrees += m_angularVelocity * m_game->m_clock->GetDeltaSeconds();
		if (m_orientationDegrees > 360.f) m_orientationDegrees -= 360.f;
	}
	if (g_theInput->IsKeyDown('F'))
	{
		//m_orientationDegrees = m_orientationDegrees - m_angularVelocity;
		m_orientationDegrees -= m_angularVelocity * m_game->m_clock->GetDeltaSeconds();
		if (m_orientationDegrees < -360.f) m_orientationDegrees += 360.f;
	}
	if (g_theInput->IsKeyDown('E'))
	{
		m_thrustFractionKeyboard = 1.f;
		m_isThrustingKeyboard = true;
	}
	else if (g_theInput->WasKeyJustReleased('E'))
	{
		m_thrustFractionKeyboard = 0.f;
		m_isThrustingKeyboard = false;
	}
	if (g_theInput->WasKeyJustPressed('N'))
	{
		if (m_health > 0)
		{
			Respawn();
		}
	}
}

void PlayerShip::UpdateFromController()
{

	XboxController const& controller = g_theInput->GetController(0);

	if (controller.WasButtonJustPressed(XboxButtonID::XBOX_BUTTON_START))
	{
		if (m_isDead && m_health > 0)
		{
			Respawn();
		}
		return;
	}

	float leftStickMagnitude = controller.GetLeftStick().GetMagnitude();
	if (leftStickMagnitude > 0.f)
	{
		m_thrustFractionController = leftStickMagnitude;
		m_orientationDegrees = controller.GetLeftStick().GetOrientationDegrees();
		m_localVerts[17].m_position = leftStickMagnitude * m_localVerts[17].m_position;
		m_localVerts[17].m_position.x = GetClamped(m_localVerts[17].m_position.x, -5.0f, -2.0f);
		m_isThrustingController = true;
	}
	else
	{
		m_thrustFractionController = 0.f;
		m_isThrustingController = false;
	}
	if (controller.WasButtonJustPressed(XboxButtonID::XBOX_BUTTON_A) || controller.GetRightTrigger() > 0.99f)
	{
		Vec2 bulletPos = Vec2(m_position.x + CosDegrees(m_orientationDegrees), m_position.y + SinDegrees(m_orientationDegrees));
		g_theInput->GetController(0).SetVibration(1000, 1000);
		m_game->SpawnBullet(bulletPos, m_orientationDegrees);
	}
}

void PlayerShip::BounceOffWalls()
{
	if (m_position.x < m_cosmeticRadius || m_position.x > WORLD_SIZE_X - m_cosmeticRadius)
		m_velocity.x = -m_velocity.x;
	if (m_position.y < m_cosmeticRadius || m_position.y > WORLD_SIZE_Y - m_cosmeticRadius)
		m_velocity.y = -m_velocity.y;

	if ((m_position.x == m_cosmeticRadius && m_position.y == m_cosmeticRadius)
		|| (m_position.x == m_cosmeticRadius && m_position.y == WORLD_SIZE_Y - m_cosmeticRadius)
		|| (m_position.x == WORLD_SIZE_X - m_cosmeticRadius && m_position.y == m_cosmeticRadius)
		|| (m_position.x == WORLD_SIZE_X - m_cosmeticRadius && m_position.y == WORLD_SIZE_Y - m_cosmeticRadius)
		)
	{
		m_velocity.x = -m_velocity.x;
		m_velocity.y = -m_velocity.y;
	}
}

void PlayerShip::Respawn()
{
	if (m_isDead)
	{
		SoundID respwan = g_theAudio->CreateOrGetSound("Data/Audio/Click.mp3");
		g_theAudio->StartSound(respwan);

		m_isDead = false;
		m_health--;
		m_velocity = Vec2(0.f, 0.f);
		m_position = Vec2(WORLD_CENTER_X, WORLD_CENTER_Y);
		m_orientationDegrees = 0.f;
	}
	
}
