#define UNUSED(x) (void)(x);
#include "Game/Game.hpp"
#include "Game/Entity.hpp"
#include "Game/App.hpp"
#include "Game/TestJob.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/FileUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Renderer/SimpleTriangleFont.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Core/JobSystem.hpp"

Vertex_PCU vertices[] = {
	Vertex_PCU(Vec2(800.f, 600.f), Rgba8(255, 255, 255, 255), Vec2(0.f, 0.f)),
	Vertex_PCU(Vec2(400.f, 200.f), Rgba8(255, 255, 255, 255), Vec2(0.f, 0.f)),
	Vertex_PCU(Vec2(1200.f, 200.f), Rgba8(255, 255, 255, 255), Vec2(0.f, 0.f)),
};


Game::Game(App* owner)
{
	UNUSED(owner);

	m_clock = new Clock(Clock::GetSystemClock());
	
	m_worldCamera.m_viewport = AABB2(Vec2(0.f, 0.f),
		Vec2((float)g_theRenderer->GetConfig().m_window->GetClientDimensions().x,
			(float)g_theRenderer->GetConfig().m_window->GetClientDimensions().y));
	m_screenCamera.m_viewport = AABB2(Vec2(0.f, 0.f),
		Vec2((float)g_theRenderer->GetConfig().m_window->GetClientDimensions().x,
			(float)g_theRenderer->GetConfig().m_window->GetClientDimensions().y));
}

Game::~Game()
{

}

void Game::Startup()
{
	LoadAssets();
	
	Vec2 worldCenter(WORLD_CENTER_X, WORLD_CENTER_Y);


	SetCamera();

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


	if (m_isOver)
	{
		m_hangingTime += m_clock->GetDeltaSeconds();
		if (m_hangingTime >= 3.f)
		{
			ReturnAttractModeVictory();
		}
	}
	
	if (!m_isPaused)
	{
		UpdateEntities();

		DeleteGarbageEntities();
	}

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

}

void Game::UpdateEntities()
{
}

void Game::LoadAssets()
{
	g_musicLibrary[MUSIC_WELCOME] = g_theAudio->CreateOrGetSound("Data/Audio/Click.mp3");
}

void Game::DeleteGarbageEntities()
{

}

void Game::KillAll()
{

}

void Game::UpdateFromKeyboard()
{
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
	if (g_theInput->WasKeyJustPressed('1')) {
		Job* job = g_theJobSystem->RetrieveJobFromCompletedQueue();
		m_retrievedJobs.push_back((TestJob*)job);
	}
	if (g_theInput->WasKeyJustPressed('2')) {
		g_theJobSystem->m_completedQueueLock.lock();
		int completedJobs = (int)g_theJobSystem->m_completedJobs.size();
		g_theJobSystem->m_completedQueueLock.unlock();
		for (int jobIndex = 0; jobIndex < completedJobs; jobIndex++) {
			Job* job = g_theJobSystem->RetrieveJobFromCompletedQueue();
			m_retrievedJobs.push_back((TestJob*)job);
		}
		//g_theJobSystem->m_completedQueueLock.unlock();
	}
	if (g_theInput->WasKeyJustPressed('O'))
	{
		m_isPaused = true;
		SetCamera();

		if (m_isAttractMode)
		{
			return;
		}

		UpdateEntities();

		DeleteGarbageEntities();

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
	if (g_theInput->WasKeyJustPressed(KEYCODE_F1))
	{
		m_isDebug = !m_isDebug;
	}
	if (g_theInput->WasKeyJustPressed('K'))
	{
		KillAll();
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
	
	g_theRenderer->SetModelConstants();
	g_theRenderer->ClearScreen(Rgba8(255, 0, 255));
	g_theRenderer->DrawVertexArray(3, vertices);
	DebugDrawRing(Vec2(SCREEN_CAMERA_SIZE_X * 0.5f, SCREEN_CAMERA_SIZE_Y * 0.5),
		(150.f + 50.f * SinDegrees(100.f * m_clock->GetTotalSeconds())),
			(20.f + 5.f * CosDegrees(100.f * m_clock->GetTotalSeconds())),
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

	g_theRenderer->ClearScreen(Rgba8(0, 0, 0));

	g_theRenderer->SetModelConstants();
	for (int jobIndex = 0; jobIndex < (int)m_retrievedJobs.size(); jobIndex++) {
		TestJob* const& job = m_retrievedJobs[jobIndex];
		if (job) {
			AABB2 aabb;
			aabb.SetCenter(Vec2((float)(job->m_coords.x) * 5.f + 2.5f, (float)(job->m_coords.y) * 5.f + 2.5f));
			aabb.SetDimensions(Vec2(4.6f, 4.6f));
			DrawSquare(aabb.m_mins, aabb.m_maxs, Rgba8::BLUE);
		}
	}

	g_theJobSystem->m_waitingQueueLock.lock();
	for (auto it = g_theJobSystem->m_waitingJobs.begin(); it != g_theJobSystem->m_waitingJobs.end(); it++) {
		TestJob* job = (TestJob*)*it;
		//job = (TestJob*)job;
		if (job) {
			AABB2 aabb;
			aabb.SetCenter(Vec2((float)(job->m_coords.x) * 5.f + 2.5f, (float)(job->m_coords.y) * 5.f + 2.5f));
			aabb.SetDimensions(Vec2(4.6f, 4.6f));
			DrawSquare(aabb.m_mins, aabb.m_maxs, Rgba8::RED);
		}
	}
	g_theJobSystem->m_waitingQueueLock.unlock();

	g_theJobSystem->m_workingQueueLock.lock();
	for (auto it = g_theJobSystem->m_workingJobs.begin(); it != g_theJobSystem->m_workingJobs.end(); it++) {
		TestJob* job = (TestJob*)*it;
		if (job) {
			AABB2 aabb;
			aabb.SetCenter(Vec2((float)(job->m_coords.x) * 5.f + 2.5f, (float)(job->m_coords.y) * 5.f + 2.5f));
			aabb.SetDimensions(Vec2(4.6f, 4.6f));
			DrawSquare(aabb.m_mins, aabb.m_maxs, Rgba8::YELLOW);
		}
	}
	g_theJobSystem->m_workingQueueLock.unlock();

	g_theJobSystem->m_completedQueueLock.lock();
	for (auto it = g_theJobSystem->m_completedJobs.begin(); it != g_theJobSystem->m_completedJobs.end(); it++) {
		TestJob* job = (TestJob*)*it;
		if (job) {
			AABB2 aabb;
			aabb.SetCenter(Vec2((float)(job->m_coords.x) * 5.f + 2.5f, (float)(job->m_coords.y) * 5.f + 2.5f));
			aabb.SetDimensions(Vec2(4.6f, 4.6f));
			DrawSquare(aabb.m_mins, aabb.m_maxs, Rgba8::GREEN);
		}
	}
	g_theJobSystem->m_completedQueueLock.unlock();

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
	//SoundID testSound = g_theAudio->CreateOrGetSound("Data/Audio/Click.mp3");
	PlayMusic(MUSIC_WELCOME);
	//g_theAudio->StartSound(testSound);
	Shutdown();
	Startup();
	for (int xIndex = 0; xIndex < 40; xIndex++) {
		for (int yIndex = 0; yIndex < 20; yIndex++) {
			TestJob* job = new TestJob(IntVec2(xIndex, yIndex));
			g_theJobSystem->AddJobToWaitingQueue(job);
		}
	}
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

