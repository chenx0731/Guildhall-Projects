#define UNUSED(x) (void)(x);
#include "Game/Game.hpp"
#include "Game/Entity.hpp"
#include "Game/App.hpp"
#include "Engine/Math/MathUtils.hpp"
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
}

Game::~Game()
{

}

void Game::Startup()
{
	Vec2 worldCenter(WORLD_CENTER_X, WORLD_CENTER_Y);

	SetCamera();

}

void Game::Update(float deltaSeconds)
{
	UpdateFromKeyboard(deltaSeconds);
	UpdateFromController(deltaSeconds);

	SetCamera();

	if (m_isAttractMode)
	{
		return;
	}


	if (m_isOver)
	{
		m_hangingTime += deltaSeconds;
		if (m_hangingTime >= 3.f)
		{
			ReturnAttractModeVictory();
		}
	}
	
	if (!m_isPaused)
	{
		UpdateEntities(deltaSeconds);

		DeleteGarbageEntities();
	}

	if (m_isShaking)
	{
		float theta = g_rng->RollRandomFloatInRange(0.f, 360.f);
		float r = g_rng->RollRandomFloatInRange(0.f, 5.f * (1.f - m_shakingTime));
		m_worldCamera.Translate2D(Vec2::MakeFromPolarDegrees(theta, r));
		m_shakingTime += deltaSeconds;
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

}

void Game::UpdateEntities(float deltaSeconds)
{
	UNUSED(deltaSeconds);
}

void Game::DeleteGarbageEntities()
{

}

void Game::KillAll()
{

}

void Game::UpdateFromKeyboard(float deltaSeconds)
{
	UNUSED(deltaSeconds);
	if (g_theInput->WasKeyJustPressed('P'))
	{
		m_isPaused = !m_isPaused;
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
		m_isPaused = true;
		SetCamera();

		if (m_isAttractMode)
		{
			return;
		}

		UpdateEntities(deltaSeconds);

		DeleteGarbageEntities();

		if (m_isShaking)
		{
			float theta = g_rng->RollRandomFloatInRange(0.f, 360.f);
			float r = g_rng->RollRandomFloatInRange(0.f, 5.f * (1.f - m_shakingTime));
			m_worldCamera.Translate2D(Vec2::MakeFromPolarDegrees(theta, r));
			m_shakingTime += deltaSeconds;
			if (m_shakingTime >= 1.f)
			{
				m_shakingTime = 0.f;
				m_isShaking = false;
			}
		}
	}
	if (g_theInput->WasKeyJustPressed(KEYCODE_F1))
	{
		m_isDebug = !m_isDebug;
	}
	if (g_theInput->WasKeyJustPressed('K'))
	{
		KillAll();
	}
}

void Game::UpdateFromController(float deltaSeconds)
{
	UNUSED(deltaSeconds);
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
	
	DebugDrawRing(Vec2(SCREEN_CAMERA_SIZE_X * 0.5f, SCREEN_CAMERA_SIZE_Y * 0.5),
		(150.f + 50.f * SinDegrees(100.f * static_cast<float>(GetCurrentTimeSeconds()))),
			(20.f + 5.f * CosDegrees(100.f * static_cast<float>(GetCurrentTimeSeconds()))),
				Rgba8(255, 150, 0));

	g_theRenderer->EndCamera(m_screenCamera);
}

void Game::RenderUI() const
{
	g_theRenderer->BeginCamera(m_screenCamera);

	g_theRenderer->EndCamera(m_screenCamera);
}


void Game::RenderEntities() const
{
	g_theRenderer->BeginCamera(m_worldCamera);

	g_theRenderer->ClearScreen(Rgba8(0, 0, 255));

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
	m_worldCamera.SetOrthoView(Vec2(0.f, 0.f), Vec2(WORLD_CAMERA_SIZE_X, WORLD_CAMERA_SIZE_Y));
	m_screenCamera.SetOrthoView(Vec2(0.f, 0.f), Vec2(SCREEN_CAMERA_SIZE_X, SCREEN_CAMERA_SIZE_Y));
}

void Game::ReturnAttractModeVictory()
{
	ReturnAttractModeNormal();
}

void Game::ReturnAttractModeFailed()
{
	ReturnAttractModeNormal();

}

void Game::ReturnAttractModeNormal()
{
	SoundID testSound = g_theAudio->CreateOrGetSound("Data/Audio/Click.mp3");
	g_theAudio->StartSound(testSound);

	m_isAttractMode = true;
}

