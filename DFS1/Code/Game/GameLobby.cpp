#define UNUSED(x) (void)(x);
#include "Game/GameLobby.hpp"
#include "Game/GamePlaying.hpp"
#include "Game/GameCommon.hpp"
#include "Game/Player.hpp"
#include "Game/App.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Core/FileUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Renderer/SimpleTriangleFont.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Core/DebugRenderSystem.hpp"

GameLobby::GameLobby()
{
	m_clock = new Clock(Clock::GetSystemClock());

	
}

GameLobby::~GameLobby()
{

}

void GameLobby::Startup()
{
	
}

void GameLobby::Update()
{
	if (m_state != GameState::LOBBY) {
		g_theGameState = m_state;
		g_theGames[(int)g_theGameState]->Enter();
	}
	SetCamera();
	UpdateFromKeyboard();
	
}

void GameLobby::Render() const
{
	//g_theRenderer->BeginCamera(m_screenCamera);

	//g_theRenderer->SetModelConstants();

	//DebugDrawRing(Vec2(SCREEN_CAMERA_SIZE_X * 0.5f, SCREEN_CAMERA_SIZE_Y * 0.5),
	//	(150.f + 50.f * SinDegrees(100.f * m_clock->GetTotalSeconds())),
	//	(20.f + 5.f * CosDegrees(100.f * m_clock->GetTotalSeconds())),
	//	Rgba8(255, 150, 0));

	for (int playerIndex = 0; playerIndex < 2; playerIndex++) {
		if (m_gamePlaying->m_players[playerIndex]) {
			Player* player = m_gamePlaying->m_players[playerIndex];
			Camera temp = m_gamePlaying->m_players[playerIndex]->m_screenCamera;
			g_theRenderer->BeginCamera(temp);
			std::vector<Vertex_PCU> texts;
			std::string playerText = Stringf("Player%d\n", playerIndex + 1);
			std::string sKeyboard = Stringf("Keyboard\nPress SPACE to start game\nPress ESCAPE to exit\n");
			std::string sController = Stringf("Controller\nPress START to start game\nPress BACK to exit\n");
			std::string wKeyboard = Stringf("Press START to join");
			std::string wController = Stringf("Press SPACE to join");
			if (player->m_type == ControllerType::KEYBOARD)
				playerText.append(sKeyboard);
			else
				playerText.append(sController);
			if (m_gamePlaying->m_playerNum == 1) {
				if (player->m_type == ControllerType::KEYBOARD)
					playerText.append(wKeyboard);
				else
					playerText.append(wController);
			}
			g_theFont->AddVertsForTextInBox2D(texts, AABB2(temp.m_bottomLeft, temp.m_topRight), 20.f, playerText);
			g_theRenderer->BindTexture(&g_theFont->GetTexture());
			g_theRenderer->DrawVertexArray((int)texts.size(), texts.data());
			g_theRenderer->BindTexture(nullptr);
			g_theRenderer->EndCamera(temp);
		}
	}


	//g_theRenderer->EndCamera(m_screenCamera);
	
}

void GameLobby::Shutdown()
{
}

void GameLobby::Enter()
{
	m_gamePlaying = (GamePlaying*)g_theGames[(int)GameState::PLAYING];
	m_state = GameState::LOBBY;

}

void GameLobby::Exit()
{
	PauseMusic(MUSIC_MAINMENU);
}


void GameLobby::SetCamera()
{
	m_screenCamera.SetOrthoView(Vec2(0.f, 0.f), Vec2(SCREEN_CAMERA_SIZE_X, SCREEN_CAMERA_SIZE_Y));
	for (int playerIndex = 0; playerIndex < 2; playerIndex++) {
		if (m_gamePlaying->m_players[playerIndex])
			m_gamePlaying->m_players[playerIndex]->m_screenCamera.SetSelfOrthoView();
		//(Vec2(0.f, 0.f), Vec2(SCREEN_CAMERA_SIZE_X, SCREEN_CAMERA_SIZE_Y));
	}
	//m_gamePlaying->m_players[0]->m_screenCamera.SetOrthoView(Vec2(0.f, 0.f), Vec2(SCREEN_CAMERA_SIZE_X, SCREEN_CAMERA_SIZE_Y));
}

void GameLobby::UpdateFromKeyboard()
{
	if (g_theInput->WasKeyJustPressed(' ')) {
		if (m_gamePlaying->m_playerNum == 1) {
			bool isAdd = false;
			for (int playerIndex = 0; playerIndex < 2; playerIndex++) {
				if (m_gamePlaying->m_players[playerIndex]) {
					if (m_gamePlaying->m_players[playerIndex]->m_type == ControllerType::CONTROLLER0) {
						m_gamePlaying->AddPlayerWithSpecController(ControllerType::KEYBOARD, m_gamePlaying->m_playerNum);
						m_gamePlaying->SetCameraByPlayerNum();
						PlayMusic(MUSIC_CLICK);
						isAdd = true;
					}
				}
			}
			if (!isAdd) {
				PlayMusic(MUSIC_CLICK);
				m_state = GameState::PLAYING;
				Exit();
				//g_theGames[(int)g_theGameState]->Enter();
			}
		}
		else
		{
			PlayMusic(MUSIC_CLICK);
			m_state = GameState::PLAYING;
			Exit();
		}
	}
	if (g_theInput->WasKeyJustPressed(KEYCODE_ESC)) {
		if (m_gamePlaying->m_playerNum == 1) {
			for (int playerIndex = 0; playerIndex < 2; playerIndex++) {
				if (m_gamePlaying->m_players[playerIndex]) {
					if (m_gamePlaying->m_players[playerIndex]->m_type == ControllerType::KEYBOARD) {
						m_gamePlaying->DeletePlayerWithSpecController(ControllerType::KEYBOARD);
						//g_theGameState = GameState::ATTRACT;
						m_state = GameState::ATTRACT;
						m_gamePlaying->m_playerNum--;
						Exit();
						PlayMusic(MUSIC_CLICK);
						//g_theInput->HandleKeyReleased(KEYCODE_ESC);
						//g_theGames[(int)g_theGameState]->Enter();
					}
				}
			}
			//m_gamePlaying->DeletePlayerWithSpecController(ControllerType::KEYBOARD);
			//g_theGameState = GameState::ATTRACT;
			//Exit();
			//g_theGames[(int)g_theGameState]->Enter();
		}
		else if (m_gamePlaying->m_playerNum == 2)
		{
			PlayMusic(MUSIC_CLICK);
			m_gamePlaying->DeletePlayerWithSpecController(ControllerType::KEYBOARD);
		}
	}
	XboxController tempController = g_theInput->GetController(0);

	if (tempController.GetButton(XBOX_BUTTON_START).WasKeyJustPressed()) {
		if (m_gamePlaying->m_playerNum == 1) {
			bool isAdd = false;
			for (int playerIndex = 0; playerIndex < 2; playerIndex++) {
				if (m_gamePlaying->m_players[playerIndex]) {
					if (m_gamePlaying->m_players[playerIndex]->m_type == ControllerType::KEYBOARD) {
						m_gamePlaying->AddPlayerWithSpecController(ControllerType::CONTROLLER0, m_gamePlaying->m_playerNum);
						m_gamePlaying->SetCameraByPlayerNum();
						isAdd = true;
						PlayMusic(MUSIC_CLICK);
					}
				}
			}
			if (!isAdd) {
				PlayMusic(MUSIC_CLICK);
				m_state = GameState::PLAYING;
				Exit();
				//g_theGames[(int)g_theGameState]->Enter();
			}
		}
		else
		{
			PlayMusic(MUSIC_CLICK);
			m_state = GameState::PLAYING;
			Exit();
			//g_theGames[(int)g_theGameState]->Enter();
		}

	}
	if (tempController.GetButton(XBOX_BUTTON_BACK).WasKeyJustPressed()) {
		if (m_gamePlaying->m_playerNum == 1) {
			for (int playerIndex = 0; playerIndex < 2; playerIndex++) {
				if (m_gamePlaying->m_players[playerIndex]) {
					if (m_gamePlaying->m_players[playerIndex]->m_type == ControllerType::CONTROLLER0) {
						m_gamePlaying->DeletePlayerWithSpecController(ControllerType::CONTROLLER0);
						m_state = GameState::ATTRACT;
						Exit();
						PlayMusic(MUSIC_CLICK);
						//g_theInput->HandleKeyReleased(XBOX_BUTTON_BACK);
						//g_theGames[(int)g_theGameState]->Enter();
					}
				}
			}
		}
		else if (m_gamePlaying->m_playerNum == 2)
		{
			PlayMusic(MUSIC_CLICK);
			m_gamePlaying->DeletePlayerWithSpecController(ControllerType::CONTROLLER0);
		}
	}
}
