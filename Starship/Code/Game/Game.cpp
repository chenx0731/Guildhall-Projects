#define UNUSED(x) (void)(x);
#include "Game/Game.hpp"
#include "Game/Entity.hpp"
#include "Game/PlayerShip.hpp"
#include "Game/Bullet.hpp"
#include "Game/Asteroid.hpp"
#include "Game/Beetle.hpp"
#include "Game/Wasp.hpp"
#include "Game/Debris.hpp"
#include "Game/App.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Renderer/SimpleTriangleFont.hpp"

Game::Game(App* owner)
{
	UNUSED(owner);

	for (int starIndex = 0; starIndex < MAX_STARS; starIndex++)
	{
		m_starsPosition[starIndex].x = g_rng->RollRandomFloatInRange(0.f, WORLD_SIZE_X);
		m_starsPosition[starIndex].y = g_rng->RollRandomFloatInRange(0.f, WORLD_SIZE_Y);
		m_starsRadius[starIndex] = g_rng->RollRandomFloatInRange(0.01f, 0.2f);
	}

	m_clock = new Clock(Clock::GetSystemClock());
}

Game::~Game()
{

}

void Game::Startup()
{
	Vec2 worldCenter(WORLD_CENTER_X, WORLD_CENTER_Y);
	m_playerShip = new PlayerShip(this, worldCenter);

	SetCamera();
	
	for (int i = 0; i < NUM_LEVEL; i++)
	{
		m_levels[i].asteroid_num = 6 + i * 3;
		m_levels[i].wasp_num = 1 + i;
		m_levels[i].beetle_num = 1 + i;
		m_levels[i].total_num = m_levels[i].asteroid_num 
								+ m_levels[i].beetle_num 
								+ m_levels[i].wasp_num;
		m_levels[i].current_kill = 0;
	}

	SpawnNewLevel();
}

void Game::Update()
{
	UpdateFromKeyboard();
	UpdateFromController();

	SetCamera();

	if (m_isAttractMode)
	{
		return;
	}

	if (m_levels[m_current_level].total_num == m_levels[m_current_level].current_kill)
	{
		m_current_level++;
		SpawnNewLevel();
	}

	if (m_isOver)
	{
		m_hangingTime += m_clock->GetDeltaSeconds();
		if (m_hangingTime >= 3.f)
		{
			ReturnAttractModeVictory();
		}
	}
	
	//if (!m_isPaused)
	//{
		UpdateEntities();

		CheckBulletsVsAsteroids();
		CheckBulletsVsBeetles();
		CheckBulletsVsWasps();

		CheckAsteroidsVsShips();
		CheckBeetlesVsShips();
		CheckWaspsVsShips();

		DeleteGarbageEntities();
	//}

	if (m_isShaking)
	{
		float theta = g_rng->RollRandomFloatInRange(0.f, 360.f);
		float r = g_rng->RollRandomFloatInRange(0.f, 5.f * (1.f - m_shakingTime));
		m_worldCamera.Translate2D(Vec2::MakeFromPolarDegrees(theta, r));
		m_shakingTime += m_clock->GetDeltaSeconds();
		if (m_shakingTime >= 1.f)
		{
			m_shakingTime = 0.f;
			m_isShaking = false;
		}
	}
	
}

void Game::Render() const
{
	if (m_isAttractMode)
	{
		RenderAttractMode();
		return;
	}

	RenderEntities();

	if (m_isDebug)
	{
		DebugRenderEntities();
	}
	
	RenderUI();

	if (m_isPaused)
	{
		RenderPaused();
	}
}

void Game::Shutdown()
{
	m_current_level = 0;

	if (m_playerShip) 
	{
		delete m_playerShip;
		m_playerShip = nullptr;
	}
	
	for (int asteroidIndex = 0; asteroidIndex < MAX_ASTEROIDS; asteroidIndex++)
	{
		Asteroid*& asteroid = m_asteroids[asteroidIndex];
		if (asteroid)
		{
			delete asteroid;
			asteroid = nullptr;
		}
	}

	for (int bulletIndex = 0; bulletIndex < MAX_BULLETS; bulletIndex++)
	{
		Bullet*& bullet = m_bullets[bulletIndex];
		if (bullet)
		{
			delete bullet;
			bullet = nullptr;
		}
	}

	for (int debrisIndex = 0; debrisIndex < MAX_DEBRIS; debrisIndex++)
	{
		Debris*& debris = m_debris[debrisIndex];
		if (debris)
		{
			delete debris;
			debris = nullptr;
		}
	}

	for (int waspIndex = 0; waspIndex < MAX_WASPS; waspIndex++)
	{
		Wasp*& wasp = m_wasps[waspIndex];
		if (wasp)
		{
			delete wasp;
			wasp = nullptr;
		}
	}

	for (int beetleIndex = 0; beetleIndex < MAX_BEETLES; beetleIndex++)
	{
		Beetle*& beetle = m_beetles[beetleIndex];
		if (beetle)
		{
			delete beetle;
			beetle = nullptr;
		}
	}
}

Asteroid* Game::SpawnRandomAsteroid()
{

	for (int asteroidIndex = 0; asteroidIndex < MAX_ASTEROIDS; asteroidIndex++)
	{
		Asteroid*& asteroid = m_asteroids[asteroidIndex];
		if (!asteroid)
		{
			asteroid = new Asteroid(this, RandomOffScreenPosition());
			asteroid->m_orientationDegrees = g_rng->RollRandomFloatInRange(0.f, 360.f);
			asteroid->m_angularVelocity = g_rng->RollRandomFloatInRange(-300.f, 300.f);
			float driftAngleDegrees = g_rng->RollRandomFloatInRange(0.f, 360.f);
			asteroid->m_velocity.x = ASTEROID_SPEED * CosDegrees(driftAngleDegrees);
			asteroid->m_velocity.y = ASTEROID_SPEED * SinDegrees(driftAngleDegrees);
			return asteroid;
		}
	}
	ERROR_RECOVERABLE("Cannot spawn a new asteroid; all slots are full!");
	return nullptr;
}

Bullet* Game::SpawnBullet(Vec2 const& pos, float forwardDegrees)
{
	if (m_playerShip->m_isDead)
		return nullptr;

	SoundID testSound = g_theAudio->CreateOrGetSound("Data/Audio/PlayerShootNormal.ogg");
	g_theAudio->StartSound(testSound);

	for (int bulletIndex = 0; bulletIndex < MAX_BULLETS; bulletIndex++)
	{
		Bullet*& bullet = m_bullets[bulletIndex];
		if (!bullet)
		{
			bullet = new Bullet(this, pos);
			bullet->m_orientationDegrees = forwardDegrees;
			bullet->m_velocity.x = BULLET_SPEED * CosDegrees(forwardDegrees);
			bullet->m_velocity.y = BULLET_SPEED * SinDegrees(forwardDegrees);
			return bullet;
		}
	}
	ERROR_RECOVERABLE("Cannot spawn a new bullet; all slots are full!");
	return nullptr;
}

Debris* Game::SpawnRandomDebris(Vec2 startPos, float minRadius, float maxRadius, Rgba8 color, float minSpeed, float maxSpeed)
{
	for (int debrisIndex = 0; debrisIndex < MAX_DEBRIS; debrisIndex++)
	{
		Debris*& debris = m_debris[debrisIndex];
		if (!debris)
		{
			debris = new Debris(this, startPos, maxRadius, minRadius, color);
			debris->m_orientationDegrees = g_rng->RollRandomFloatInRange(0.f, 360.f);
			debris->m_angularVelocity = g_rng->RollRandomFloatInRange(-360.f, 360.f);
			float speed = g_rng->RollRandomFloatInRange(minSpeed, maxSpeed);
			debris->m_velocity = Vec2(speed * CosDegrees(debris->m_orientationDegrees), speed * SinDegrees(debris->m_orientationDegrees));
			return debris;
		}
	}
	
	return nullptr;
}

Wasp* Game::SpawnRandomWasp()
{
	for (int waspIndex = 0; waspIndex < MAX_WASPS; waspIndex++)
	{
		Wasp*& wasp = m_wasps[waspIndex];
		if (!wasp)
		{
			wasp = new Wasp(this, RandomOffScreenPosition());
			return wasp;
		}
	}
	return nullptr;
}

Beetle* Game::SpawnRandomBeetle()
{
	for (int beetleIndex = 0; beetleIndex < MAX_BEETLES; beetleIndex++)
	{
		Beetle*& beetle = m_beetles[beetleIndex];
		if (!beetle)
		{
			beetle = new Beetle(this, RandomOffScreenPosition());
			return beetle;
		}
	}
	return nullptr;
}

void Game::SpawnNewLevel()
{
	if (m_current_level != 0)
	{
		SoundID testSound = g_theAudio->CreateOrGetSound("Data/Audio/NewLevel.mp3");
		g_theAudio->StartSound(testSound);
	}
	if (m_current_level >= 5)
	{
		//ReturnAttractModeVictory();
		m_isOver = true;
		return;
	}
	for (int i = 0; i < m_levels[m_current_level].asteroid_num; i++)
	{
		SpawnRandomAsteroid();
	}
	for (int i = 0; i < m_levels[m_current_level].wasp_num; i++)
	{
		SpawnRandomWasp();
	}
	for (int i = 0; i < m_levels[m_current_level].beetle_num; i++)
	{
		SpawnRandomBeetle();
	}
}

void Game::UpdateEntities()
{
	for (int bulletIndex = 0; bulletIndex < MAX_BULLETS; bulletIndex++)
	{
		Bullet*& bullet = m_bullets[bulletIndex];
		if (bullet)
		{
			bullet->Update();
		}
	}

	for (int asteroidIndex = 0; asteroidIndex < MAX_ASTEROIDS; asteroidIndex++)
	{
		Asteroid*& asteroid = m_asteroids[asteroidIndex];
		if (asteroid)
		{
			asteroid->Update();
		}
	}
	for (int debrisIndex = 0; debrisIndex < MAX_DEBRIS; debrisIndex++)
	{
		Debris*& debris = m_debris[debrisIndex];
		if (debris)
		{
			debris->Update();
		}
	}
	for (int waspIndex = 0; waspIndex < MAX_WASPS; waspIndex++)
	{
		Wasp*& wasp = m_wasps[waspIndex];
		if (wasp)
		{
			wasp->Update();
		}
	}
	for (int beetleIndex = 0; beetleIndex < MAX_BEETLES; beetleIndex++)
	{
		Beetle*& beetle = m_beetles[beetleIndex];
		if (beetle)
		{
			beetle->Update();
		}
	}

	PlayerShip*& playerShip = m_playerShip;
	playerShip->Update();
}

void Game::CheckBulletsVsAsteroids()
{
	for (int asterIndex = 0; asterIndex < MAX_ASTEROIDS; asterIndex++)
	{
		if (m_asteroids[asterIndex])
		{
			for (int bulletIndex = 0; bulletIndex < MAX_BULLETS; bulletIndex++)
			{
				if (m_bullets[bulletIndex])
					CheckBulletVsAsteroid(*m_bullets[bulletIndex], *m_asteroids[asterIndex]);
			}
		}
	}
}

void Game::CheckBulletVsAsteroid(Bullet& bullet, Asteroid& asteroid)
{
	bool checkIsCollided = DoDiscsOverlap(bullet.m_position, bullet.m_physicsRadius, asteroid.m_position, asteroid.m_physicsRadius);
	if (checkIsCollided)
	{
		bullet.Die();
		asteroid.Damege(bullet.m_position);
	}
}

void Game::CheckBulletsVsBeetles()
{
	for (int beetleIndex = 0; beetleIndex < MAX_BEETLES; beetleIndex++)
	{
		if (m_beetles[beetleIndex])
		{
			for (int bulletIndex = 0; bulletIndex < MAX_BULLETS; bulletIndex++)
			{
				if (m_bullets[bulletIndex])
					CheckBulletVsBeetle(*m_bullets[bulletIndex], *m_beetles[beetleIndex]);
			}
		}
	}
}

void Game::CheckBulletVsBeetle(Bullet& bullet, Beetle& beetle)
{
	bool CheckIsCollided = DoDiscsOverlap(bullet.m_position, bullet.m_physicsRadius, beetle.m_position, beetle.m_physicsRadius);
	if (CheckIsCollided)
	{
		bullet.Die();
		beetle.Damege(bullet.m_position);
	}
}

void Game::CheckBulletsVsWasps()
{
	for (int waspIndex = 0; waspIndex < MAX_WASPS; waspIndex++)
	{
		if (m_wasps[waspIndex])
		{
			for (int bulletIndex = 0; bulletIndex < MAX_BULLETS; bulletIndex++)
			{
				if (m_bullets[bulletIndex])
					CheckBulletVsWasp(*m_bullets[bulletIndex], *m_wasps[waspIndex]);
			}
		}
	}
}

void Game::CheckBulletVsWasp(Bullet& bullet, Wasp& wasp)
{
	bool CheckIsCollided = DoDiscsOverlap(bullet.m_position, bullet.m_physicsRadius, wasp.m_position, wasp.m_physicsRadius);
	if (CheckIsCollided)
	{
		bullet.Die();
		wasp.Damege(bullet.m_position);
	}
}

void Game::CheckAsteroidsVsShips()
{
	if (m_playerShip->m_isDead)
		return;
	for (int asterIndex = 0; asterIndex < MAX_ASTEROIDS; asterIndex++)
	{
		if (m_asteroids[asterIndex])
		{
			CheckAsteroidVsShip(*m_asteroids[asterIndex], *m_playerShip);
		}
	}
}

void Game::CheckAsteroidVsShip(Asteroid& asteroid, PlayerShip& ship)
{
	bool checkIsCollided = DoDiscsOverlap(asteroid.m_position, asteroid.m_physicsRadius, ship.m_position, ship.m_physicsRadius);
	if (checkIsCollided)
	{
		ship.Die();
		//asteroid.Damege();
	}
}

void Game::CheckBeetlesVsShips()
{
	if (m_playerShip->m_isDead)
		return;
	for (int beetleIndex = 0; beetleIndex < MAX_BEETLES; beetleIndex++)
	{
		if (m_beetles[beetleIndex])
		{
			CheckBeetleVsShip(*m_beetles[beetleIndex], *m_playerShip);
		}
	}
}

void Game::CheckBeetleVsShip(Beetle& beetle, PlayerShip& ship)
{
	bool checkIsCollided = DoDiscsOverlap(beetle.m_position, beetle.m_physicsRadius, ship.m_position, ship.m_physicsRadius);
	if (checkIsCollided)
	{
		ship.Die();
		//beetle.Damege();
	}
}

void Game::CheckWaspsVsShips()
{
	if (m_playerShip->m_isDead)
		return;
	for (int waspIndex = 0; waspIndex < MAX_WASPS; waspIndex++)
	{
		if (m_wasps[waspIndex])
		{
			CheckWaspVsShip(*m_wasps[waspIndex], *m_playerShip);
		}
	}
}

void Game::CheckWaspVsShip(Wasp& wasp, PlayerShip& ship)
{
	bool checkIsCollided = DoDiscsOverlap(wasp.m_position, wasp.m_physicsRadius, ship.m_position, ship.m_physicsRadius);
	if (checkIsCollided)
	{
		ship.Die();
		//wasp.Damege();
	}
}

void Game::DeleteGarbageEntities()
{
	for (int asteroidIndex = 0; asteroidIndex < MAX_ASTEROIDS; asteroidIndex++)
	{
		Asteroid*& asteroid = m_asteroids[asteroidIndex];
		if (asteroid && asteroid->m_isGarbage)
		{
			delete asteroid;
			asteroid = nullptr;
			m_levels[m_current_level].current_kill++;
		}
	}

	for (int bulletIndex = 0; bulletIndex < MAX_BULLETS; bulletIndex++)
	{
		Bullet*& bullet = m_bullets[bulletIndex];
		if (bullet && bullet->m_isGarbage)
		{
			delete bullet;
			bullet = nullptr;
		}
	}

	for (int debrisIndex = 0; debrisIndex < MAX_DEBRIS; debrisIndex++)
	{
		Debris*& debris = m_debris[debrisIndex];
		if (debris && debris->m_isGarbage)
		{
			delete debris;
			debris = nullptr;
		}
	}

	for (int waspIndex = 0; waspIndex < MAX_WASPS; waspIndex++)
	{
		Wasp*& wasp = m_wasps[waspIndex];
		if (wasp && wasp->m_isGarbage)
		{
			delete wasp;
			wasp = nullptr;
			m_levels[m_current_level].current_kill++;
		}
	}

	for (int beetleIndex = 0; beetleIndex < MAX_BEETLES; beetleIndex++)
	{
		Beetle*& beetle = m_beetles[beetleIndex];
		if (beetle && beetle->m_isGarbage)
		{
			delete beetle;
			beetle = nullptr;
			m_levels[m_current_level].current_kill++;
		}
	}
}

void Game::KillAll()
{
	for (int asteroidIndex = 0; asteroidIndex < MAX_ASTEROIDS; asteroidIndex++)
	{
		Asteroid*& asteroid = m_asteroids[asteroidIndex];
		if (asteroid)
		{
			asteroid->Die();
		}
	}

	for (int waspIndex = 0; waspIndex < MAX_WASPS; waspIndex++)
	{
		Wasp*& wasp = m_wasps[waspIndex];
		if (wasp)
		{
			/*delete wasp;
			wasp = nullptr;*/
			wasp->Die();
		}
	}

	for (int beetleIndex = 0; beetleIndex < MAX_BEETLES; beetleIndex++)
	{
		Beetle*& beetle = m_beetles[beetleIndex];
		if (beetle)
		{
			/*delete beetle;
			beetle = nullptr;*/
			beetle->Die();
		}
	}
}

void Game::UpdateFromKeyboard()
{
	if (g_theInput->WasKeyJustPressed('P'))
	{
		//m_isPaused = !m_isPaused;
		m_clock->TogglePause();
	}
	if (g_theInput->WasKeyJustPressed(' ') || g_theInput->WasKeyJustPressed('N'))
	{
		if (m_isAttractMode)
		{
			EnterGame();
		}
			
	}
	if (g_theInput->WasKeyJustPressed('O'))
	{
		m_clock->StepSingleFrame();
	}
	if (g_theInput->WasKeyJustPressed(KEYCODE_F1))
	{
		m_isDebug = !m_isDebug;
	}
	if (g_theInput->WasKeyJustPressed('K'))
	{
		KillAll();
	}
	if (g_theInput->IsKeyDown('T'))
	{
		m_clock->SetTimeScale(0.1f);
	}
	if (g_theInput->WasKeyJustReleased('T'))
	{
		m_clock->SetTimeScale(1);
	}
}

void Game::UpdateFromController()
{
	if (g_theInput->GetController(0).WasButtonJustPressed(XBOX_BUTTON_START) 
		|| g_theInput->GetController(0).WasButtonJustPressed(XBOX_BUTTON_A))
	{
		if (m_isAttractMode)
		{
			EnterGame();
		}
	}
}

void Game::RenderAttractMode() const
{
	g_theRenderer->BeginCamera(m_screenCamera);

	//
	DrawStarCurves(Vec2(100.f, 900.f), 650.f, 10.f, 0.f, 200.f, 250.f, 310.f, Rgba8(191, 209, 251));
	DrawStarCurves(Vec2(100.f, 900.f), 510.f, 10.f, 0.f, 500.f, 200.f, 310.f, Rgba8(5, 9, 120));
	DrawStarCurves(Vec2(100.f, 900.f), 1000.f, 10.f, 0.f, 410.f, 200.f, 310.f, Rgba8(5, 9, 129));

	DrawStarCurves(Vec2(905.f, 145.f), 650.f, 10.f, 0.f, 200.f, 90.f, 145.f, Rgba8(191, 209, 251));
	DrawStarCurves(Vec2(905.f, 145.f), 1000.f, 10.f, 0.f, 400.f, 90.f, 145.f, Rgba8(5, 9, 129));
	DrawStarCurves(Vec2(905.f, 145.f), 500.f, 10.f, 0.f, 500.f, 90.f, 180.f, Rgba8(5, 9, 120));

	DrawStarCurves(Vec2(900.f, 545.f), 200.f, 10.f, 0.f, 100.f, 0.f, 90.f, Rgba8(191, 209, 251));
	DrawStarCurves(Vec2(900.f, 545.f), 500.f, 10.f, 0.f, 300.f, 0.f, 90.f, Rgba8(5, 9, 129));

	DrawStarCurves(Vec2(900.f, 560.f), 200.f, 10.f, 0.f, 100.f, 260.f, 360.f, Rgba8(191, 209, 251));
	DrawStarCurves(Vec2(900.f, 560.f), 500.f, 10.f, 0.f, 300.f, 260.f, 360.f, Rgba8(5, 9, 150));

	DrawStarCurves(Vec2(890.f, 505.f), 150.f, 10.f, 0.f, 150.f, 0.f, 260.f, Rgba8(191, 209, 251));
	DrawStarCurves(Vec2(890.f, 505.f), 150.f, 10.f, 0.f, 150.f, 300.f, 360.f, Rgba8(191, 209, 251));
	DrawStarCurves(Vec2(890.f, 505.f), 50.f, 10.f, 0.f, 50.f, 0.f, 360.f, Rgba8(5, 9, 129));

	DrawStarCurves(Vec2(500.f, -1000.f), 1200.f, 10.f, 0.f, 100.f, 70.f, 150.f, Rgba8(234, 228, 138));
	DrawStarCurves(Vec2(500.f, -1000.f), 1100.f, 10.f, 0.f, 100.f, 70.f, 150.f, Rgba8(5, 9, 70));

	DrawStarCurves(Vec2(1500.f, -900.f), 1200.f, 10.f, 0.f, 100.f, 80.f, 125.f, Rgba8(234, 228, 138));
	DrawStarCurves(Vec2(1500.f, -900.f), 1150.f, 10.f, 0.f, 300.f, 80.f, 125.f, Rgba8(5, 9, 70));
	DrawStarCurves(Vec2(1500.f, -900.f), 2000.f, 10.f, 0.f, 800.f, 80.f, 100.f, Rgba8(5, 9, 129));

	//DrawStarCurves(Vec2(1300.f, -900.f), 1200.f, 10.f, 0.f, 200.f, 0.f, 180.f, Rgba8(191, 209, 251));

	// Stars
	DrawCircle(Vec2(600.f, 380.f), 30.f, Rgba8(255, 200, 0));
	DrawStarCircle(Vec2(600.f, 380.f), 50.f, 2.f, 0.f, Rgba8(255, 255, 200));
	//DrawStarCurves(Vec2(600.f, 380.f), 70.f, 10.f, 0.f, 30.f, 0.f, 360.f, Rgba8(54, 111, 190));

	DrawCircle(Vec2(550.f, 750.f), 18.f, Rgba8(255, 180, 0));
	DrawStarCircle(Vec2(550.f, 750.f), 40.f, 4.f, 0.f, Rgba8(255, 255, 200));
	//DrawStarCurves(Vec2(550.f, 750.f), 50.f, 10.f, 0.f, 20.f, 0.f, 360.f, Rgba8(54, 111, 190));

	DrawCircle(Vec2(100.f, 450.f), 10.f, Rgba8(255, 220, 0));
	DrawStarCircle(Vec2(100.f, 450.f), 30.f, 2.f, 0.f, Rgba8(255, 255, 200));
	//DrawStarCurves(Vec2(100.f, 450.f), 40.f, 10.f, 0.f, 20.f, 0.f, 360.f, Rgba8(54, 111, 190));

	DrawCircle(Vec2(150.f, 600.f), 20.f, Rgba8(255, 220, 0));
	DrawStarCircle(Vec2(150.f, 600.f), 35.f, 2.f, 0.f, Rgba8(255, 255, 200));
	//DrawStarCurves(Vec2(150.f, 600.f), 40.f, 10.f, 0.f, 10.f, 0.f, 360.f, Rgba8(54, 111, 190));

	DrawCircle(Vec2(1150.f, 730.f), 25.f, Rgba8(255, 190, 5));
	DrawStarCircle(Vec2(1150.f, 730.f), 35.f, 2.f, 0.f, Rgba8(255, 255, 200));
	//DrawStarCurves(Vec2(1150.f, 730.f), 45.f, 10.f, 0.f, 15.f, 0.f, 360.f, Rgba8(54, 111, 190));

	DrawCircle(Vec2(1250.f, 600.f), 20.f, Rgba8(255, 190, 5));
	DrawStarCircle(Vec2(1250.f, 600.f), 30.f, 2.f, 0.f, Rgba8(255, 255, 200));
	//DrawStarCurves(Vec2(1250.f, 600.f), 40.f, 10.f, 0.f, 15.f, 0.f, 360.f, Rgba8(54, 111, 190));

	DrawCircle(Vec2(1400.f, 720.f), 20.f, Rgba8(255, 190, 5));
	DrawStarCircle(Vec2(1400.f, 720.f), 80.f, 2.f, 0.f, Rgba8(255, 255, 200));
	DrawCircle(Vec2(1400.f, 720.f), 40.f, Rgba8(255, 190, 5));
	DrawStarCircle(Vec2(1415.f, 723.f), 27.f, 2.f, 0.f, Rgba8(255, 255, 200));
	DrawStarCurves(Vec2(1400.f, 720.f), 120.f, 10.f, 0.f, 60.f, 0.f, 360.f, Rgba8(54, 111, 190));
	
	g_theRenderer->EndCamera(m_screenCamera);
}

void Game::RenderUI() const
{
	g_theRenderer->BeginCamera(m_screenCamera);

	Vertex_PCU lifePlayershipVerts[NUM_SHIP_VERTS - 3];
	PlayerShip::CopyPlayershipVerts(lifePlayershipVerts);
	for (int lifeIndex = 0; lifeIndex < m_playerShip->m_health; lifeIndex++)
	{
		/*if (lifeIndex == 3)
		{
			break;
		}*/
		Vec2 shift = Vec2(24.f, 776.f);
		float orientation = 90.f;
		float scale = 0.8f * 8.f;
		if (lifeIndex > 0)
		{
			shift = Vec2(5.f * 8.f, 0.f);
			orientation = 0.f;
			scale = 1.f;
		}
		TransformVertexArrayXY3D(NUM_SHIP_VERTS - 3, lifePlayershipVerts, scale, orientation, shift);

		g_theRenderer->DrawVertexArray(NUM_SHIP_VERTS - 3, lifePlayershipVerts);
	}

	RenderMinimap();

	g_theRenderer->EndCamera(m_screenCamera);
}

void Game::RenderMinimap() const
{
	float startX = 1300.f;
	float endX = 1500.f;
	float startY = 625.f;
	float endY = 725.f;
	float gapX = 20.f;
	float gapY = 10.f;
	float rangeX = 400.f;
	float rangeY = 200.f;
	float maxX = m_playerShip->m_position.x + rangeX * 0.5f;
	float minX = m_playerShip->m_position.x - rangeX * 0.5f;
	float maxY = m_playerShip->m_position.y + rangeY * 0.5f;
	float minY = m_playerShip->m_position.y - rangeY * 0.5f;
	// Background
	DrawSquare(Vec2(startX, startY), Vec2(endX, endY), Rgba8(0, 0, 0));

	// Frame
	for (int i = 0; i < 11; i++)
	{
		float index = static_cast<float> (i);
		Rgba8 color = Rgba8(50, 50, 50);
		float thickness = 1.6f;
		if (i == 0 || i == 10)
		{
			color = Rgba8(127, 127, 127);
			thickness = 3.2f;
		}
		DebugDrawLine(Vec2(startX + index * gapX, startY), Vec2(startX + index * gapX, endY), thickness, color);
		DebugDrawLine(Vec2(startX, startY + index * gapY), Vec2(endX, startY + index * gapY), thickness, color);
	}

	// Entities
	bool isEnemyUp = false;
	bool isEnemyDown = false;
	bool isEnemyRight = false;
	bool isEnemyLeft = false;

	for (int asteroidIndex = 0; asteroidIndex < MAX_ASTEROIDS; asteroidIndex++)
	{
		Asteroid const* asteroid = m_asteroids[asteroidIndex];
		if (asteroid)
		{
			float positionX = RangeMap(asteroid->m_position.x, minX, maxX, startX, endX);
			float positionY = RangeMap(asteroid->m_position.y, minY, maxY, startY, endY);
			if (positionX < startX || positionY < startY || positionX > endX || positionY > endY)
			{
				if (positionX < startX)
					isEnemyLeft = true;
				if (positionX > endX)
					isEnemyRight = true;
				if (positionY < startY)
					isEnemyDown = true;
				if (positionY > endY)
					isEnemyUp = true;
				continue;
			}
			Vec2 position = Vec2(positionX, positionY);
			DrawCircle(position, 4.f, Rgba8(ASTEROID_COLOR_R, ASTEROID_COLOR_G, ASTEROID_COLOR_B));
		}
	}
	for (int beetleIndex = 0; beetleIndex < MAX_BEETLES; beetleIndex++)
	{
		Beetle const* beetle = m_beetles[beetleIndex];
		if (beetle)
		{
			float positionX = RangeMap(beetle->m_position.x, minX, maxX, startX, endX);
			float positionY = RangeMap(beetle->m_position.y, minY, maxY, startY, endY);
			if (positionX < startX || positionY < startY || positionX > endX || positionY > endY)
			{
				if (positionX < startX)
					isEnemyLeft = true;
				if (positionX > endX)
					isEnemyRight = true;
				if (positionY < startY)
					isEnemyDown = true;
				if (positionY > endY)
					isEnemyUp = true;
				continue;
			}
			Vec2 position = Vec2(positionX, positionY);
			DrawCircle(position, 4.f, Rgba8(BEETLE_COLOR_R, BEETLE_COLOR_G, BEETLE_COLOR_B));
		}
	}

	for (int waspIndex = 0; waspIndex < MAX_WASPS; waspIndex++)
	{
		Wasp const* wasp = m_wasps[waspIndex];
		if (wasp)
		{
			float positionX = RangeMap(wasp->m_position.x, minX, maxX, startX, endX);
			float positionY = RangeMap(wasp->m_position.y, minY, maxY, startY, endY);
			if (positionX < startX || positionY < startY || positionX > endX || positionY > endY)
			{
				if (positionX < startX)
					isEnemyLeft = true;
				if (positionX > endX)
					isEnemyRight = true;
				if (positionY < startY)
					isEnemyDown = true;
				if (positionY > endY)
					isEnemyUp = true;
				continue;
			}
			Vec2 position = Vec2(positionX, positionY);
			DrawCircle(position, 4.f, Rgba8(WASP_COLOR_R, WASP_COLOR_G, WASP_COLOR_B));
		}
	}

	if (isEnemyUp == true)
	{
		DebugDrawLine(Vec2(startX, endY), Vec2(endX, endY), 3.2f, Rgba8(255, 0, 0));
	}
	if (isEnemyDown == true)
	{
		DebugDrawLine(Vec2(startX, startY), Vec2(endX, startY), 3.2f, Rgba8(255, 0, 0));
	}
	if (isEnemyLeft == true)
	{
		DebugDrawLine(Vec2(startX, startY), Vec2(startX, endY),3.2f, Rgba8(255, 0, 0));
	}
	if (isEnemyRight == true)
	{
		DebugDrawLine(Vec2(endX, startY), Vec2(endX, endY), 3.2f, Rgba8(255, 0, 0));
	}

	std::vector<Vertex_PCU> testTextVerts;
	Level nowLevel = m_levels[m_current_level];
	int remainNum = nowLevel.total_num - nowLevel.current_kill;
	if (remainNum < 0)
		remainNum = 0;
	AddVertsForTextTriangles2D(testTextVerts, "Illegal Objects: " + std::to_string(remainNum), Vec2(-5.f, -0.5f), 1.f, Rgba8(255, 255, 255));
	TransformVertexArrayXY3D(static_cast<int>(testTextVerts.size()), testTextVerts.data(), 16.f, 0.f, Vec2(startX + 80.f, startY - 25.f));
	g_theRenderer->DrawVertexArray(static_cast<int>(testTextVerts.size()), &testTextVerts[0]);

	//m_playerShip->Render();
	if (m_playerShip->m_isDead)
		return;
	float positionX = RangeMap(m_playerShip->m_position.x, minX, maxX, startX, endX);
	float positionY = RangeMap(m_playerShip->m_position.y, minY, maxY, startY, endY);
	if (positionX < startX || positionY < startY || positionX > endX || positionY > endY)
	{
		return;
	}
	Vec2 position = Vec2(positionX, positionY);
	DrawCircle(position, 4.f, Rgba8(PLAYER_SHIP_COLOR_R, PLAYER_SHIP_COLOR_G, PLAYER_SHIP_COLOR_B));
}

void Game::RenderEntities() const
{
	g_theRenderer->BeginCamera(m_worldCamera);

	for (int starIndex = 0; starIndex < MAX_STARS; starIndex++)
	{
		Rgba8 colorA = Rgba8(255, 255, 255);
		Rgba8 colorB = Rgba8(255, 255, 255, static_cast<unsigned char>(255.f * fabs( SinDegrees(10.f * static_cast<float>(GetCurrentTimeSeconds())))));
		Rgba8 starColor;
		float nowX = m_starsPosition[starIndex].x + 0.05f * (m_playerShip->m_position.x - WORLD_CENTER_X);
		float nowY = m_starsPosition[starIndex].y + 0.05f * (m_playerShip->m_position.y - WORLD_CENTER_Y);
		if (starIndex % 2)
		{
			starColor = colorA;
			nowX = m_starsPosition[starIndex].x + 0.02f * (m_playerShip->m_position.x - WORLD_CENTER_X);
			nowY = m_starsPosition[starIndex].y + 0.02f * (m_playerShip->m_position.y - WORLD_CENTER_Y);
		}
		else
		{
			starColor = colorB;
		}
		DrawCircle(Vec2(nowX, nowY), m_starsRadius[starIndex], starColor);
	}

	for (int bulletIndex = 0; bulletIndex < MAX_BULLETS; bulletIndex++)
	{
		Bullet const* bullet = m_bullets[bulletIndex];
		if (bullet)
		{
			bullet->Render();
		}
	}

	for (int asteroidIndex = 0; asteroidIndex < MAX_ASTEROIDS; asteroidIndex++)
	{
		Asteroid const* asteroid = m_asteroids[asteroidIndex];
		if (asteroid)
		{
			asteroid->Render();
		}
	}

	for (int debrisIndex = 0; debrisIndex < MAX_DEBRIS; debrisIndex++)
	{
		Debris const* debris = m_debris[debrisIndex];
		if (debris)
		{
			debris->Render();
		}
	}

	for (int beetleIndex = 0; beetleIndex < MAX_BEETLES; beetleIndex++)
	{
		Beetle const* beetle = m_beetles[beetleIndex];
		if (beetle)
		{
			beetle->Render();
		}
	}

	for (int waspIndex = 0; waspIndex < MAX_WASPS; waspIndex++)
	{
		Wasp const* wasp = m_wasps[waspIndex];
		if (wasp)
		{
			wasp->Render();
		}
	}

	m_playerShip->Render();

	g_theRenderer->EndCamera(m_worldCamera);
}

void Game::RenderPaused() const
{
	g_theRenderer->BeginCamera(m_screenCamera);
	DrawSquare(m_screenCamera.m_bottomLeft, m_screenCamera.m_topRight, Rgba8(127, 127, 127, 127));
	g_theRenderer->EndCamera(m_screenCamera);
}

void Game::DebugRenderEntities() const
{
	g_theRenderer->BeginCamera(m_worldCamera);
	PlayerShip const* playerShip = m_playerShip;
	playerShip->DebugRender();
	for (int bulletIndex = 0; bulletIndex < MAX_BULLETS; bulletIndex++)
	{
		Bullet const* bullet = m_bullets[bulletIndex];
		if (bullet)
		{
			bullet->DebugRender();
			DebugDrawLine(playerShip->m_position, bullet->m_position, 0.2f, Rgba8(50, 50, 50));
		}
	}

	for (int asteroidIndex = 0; asteroidIndex < MAX_ASTEROIDS; asteroidIndex++)
	{
		Asteroid const* asteroid = m_asteroids[asteroidIndex];
		if (asteroid)
		{
			asteroid->DebugRender();
			DebugDrawLine(playerShip->m_position, asteroid->m_position, 0.2f, Rgba8(50, 50, 50));
		}
	}

	for (int debrisIndex = 0; debrisIndex < MAX_DEBRIS; debrisIndex++)
	{
		Debris const* debris = m_debris[debrisIndex];
		if (debris)
		{
			debris->DebugRender();
			DebugDrawLine(playerShip->m_position, debris->m_position, 0.2f, Rgba8(50, 50, 50));
		}
	}

	for (int beetleIndex = 0; beetleIndex < MAX_BEETLES; beetleIndex++)
	{
		Beetle const* beetle = m_beetles[beetleIndex];
		if (beetle)
		{
			beetle->DebugRender();
			DebugDrawLine(playerShip->m_position, beetle->m_position, 0.2f, Rgba8(50, 50, 50));
		}
	}

	for (int waspIndex = 0; waspIndex < MAX_WASPS; waspIndex++)
	{
		Wasp const* wasp = m_wasps[waspIndex];
		if (wasp)
		{
			wasp->DebugRender();
			DebugDrawLine(playerShip->m_position, wasp->m_position, 0.2f, Rgba8(50, 50, 50));
		}
	}
	g_theRenderer->EndCamera(m_worldCamera);
}

void Game::EnterGame()
{
	SoundID testSound = g_theAudio->CreateOrGetSound("Data/Audio/Click.mp3");
	g_theAudio->StartSound(testSound);
	Shutdown();
	Startup();
	m_isAttractMode = false;
	g_theInput->EndFrame();
}

void Game::SetCamera()
{
	float worldCamera_minX = m_playerShip->m_position.x - WORLD_CAMERA_SIZE_X * 0.5f;
	float worldCamera_maxX = m_playerShip->m_position.x + WORLD_CAMERA_SIZE_X * 0.5f;
	float worldCamera_minY = m_playerShip->m_position.y - WORLD_CAMERA_SIZE_Y * 0.5f;
	float worldCamera_maxY = m_playerShip->m_position.y + WORLD_CAMERA_SIZE_Y * 0.5f;

	if (worldCamera_minX < 0.f)
	{
		worldCamera_minX = 0.f;
		worldCamera_maxX = worldCamera_minX + WORLD_CAMERA_SIZE_X;
	}
	if (worldCamera_maxX > WORLD_SIZE_X)
	{
		worldCamera_maxX = WORLD_SIZE_X;
		worldCamera_minX = WORLD_SIZE_X - WORLD_CAMERA_SIZE_X;
	}
	if (worldCamera_minY < 0.f)
	{
		worldCamera_minY = 0.f;
		worldCamera_maxY = worldCamera_minY + WORLD_CAMERA_SIZE_Y;
	}
	if (worldCamera_maxY > WORLD_SIZE_Y)
	{
		worldCamera_maxY = WORLD_SIZE_Y;
		worldCamera_minY = WORLD_SIZE_Y - WORLD_CAMERA_SIZE_Y;
	}
	
	m_worldCamera.SetOrthoView(Vec2(worldCamera_minX, worldCamera_minY), Vec2(worldCamera_maxX, worldCamera_maxY));
	m_screenCamera.SetOrthoView(Vec2(0.f, 0.f), Vec2(SCREEN_CAMERA_SIZE_X, SCREEN_CAMERA_SIZE_Y));
}

void Game::ReturnAttractModeVictory()
{
	ReturnAttractModeNormal();
	SoundID testSound = g_theAudio->CreateOrGetSound("Data/Audio/YouWin.mp3");
	g_theAudio->StartSound(testSound);
}

void Game::ReturnAttractModeFailed()
{
	ReturnAttractModeNormal();
	SoundID testSound = g_theAudio->CreateOrGetSound("Data/Audio/YouLose.mp3");
	g_theAudio->StartSound(testSound);
}

void Game::ReturnAttractModeNormal()
{
	SoundID testSound = g_theAudio->CreateOrGetSound("Data/Audio/Click.mp3");
	g_theAudio->StartSound(testSound);

	m_isAttractMode = true;
}

