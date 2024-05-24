#define UNUSED(x) (void)(x);
#include "Game/GamePlaying.hpp"
#include "Game/Entity.hpp"
#include "Game/Player.hpp"
#include "Game/Prop.hpp"
#include "Game/Unit.hpp"
#include "Game/App.hpp"
#include "Game/Map.hpp"
#include "Game/Model.hpp"
#include "Game/MapDefinition.hpp"
#include "Game/TileDefinition.hpp"
#include "Game/UnitDefinition.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Core/FileUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Renderer/SimpleTriangleFont.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Core/DebugRenderSystem.hpp"
#include "Engine/Core/FileUtils.hpp"
#include "Engine/Renderer/Window.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Math/RaycastResult2D.hpp"
#include "Engine/Math/Mat44.hpp"
#include "Engine/UI/Button.hpp"
#include "Engine/Net/NetSystem.hpp"


bool Command_LoadMap(EventArgs& args)
{
	std::string name = args.GetValue("name", "");
	const MapDefinition* def = MapDefinition::GetMapDef(name);
	if (def == nullptr) {
		g_theDevConsole->AddLine(DevConsole::ERROR, Stringf("Can't find Map: %s", name.c_str()));
		return true;
	}
	if (g_theGame->m_map != nullptr) {
		delete g_theGame->m_map;
		g_theGame->m_map = nullptr;
	}
	g_theGame->m_map = new Map(g_theGame, name.c_str());
	return true;
}

bool Command_GetReady(EventArgs& args)
{
	UNUSED(args);
	g_theGame->m_isRemoteReady = true;
	Player* remote = g_theGame->GetRemotePlayer();
	if (remote->m_state == PlayerState::Connected)
		remote->m_state = PlayerState::Ready;
	return true;
}

bool Command_StartTurn(EventArgs& args)
{
	UNUSED(args);
	g_theGame->StartTurn();
	return true;
}

bool Command_EndTurn(EventArgs& args)
{
	UNUSED(args);
	g_theGame->EndTurn();
	return true;
}

bool Command_ConfirmEndTurn(EventArgs& args)
{
	UNUSED(args);
	g_theGame->ConfirmEndTurn();
	return true;
}

bool Command_Select(EventArgs& args)
{
	UNUSED(args);
	g_theGame->Select();
	return true;
}

bool Command_SelectLeft(EventArgs& args)
{
	UNUSED(args);
	g_theGame->SelectLeft();
	return true;
}

bool Command_SelectRight(EventArgs& args)
{
	UNUSED(args);
	g_theGame->SelectRight();
	return true;
}

bool Command_Cancel(EventArgs& args)
{
	UNUSED(args);
	g_theGame->Cancel();
	return true;
}

bool Command_Move(EventArgs& args)
{
	UNUSED(args);
	g_theGame->Move();
	return true;
}

bool Command_ConfirmMove(EventArgs& args)
{
	UNUSED(args);
	g_theGame->ConfirmMove();
	return true;
}

bool Command_Attack(EventArgs& args)
{
	UNUSED(args);
	g_theGame->Attack();
	return true;
}

bool Command_ConfirmAttack(EventArgs& args)
{
	UNUSED(args);
	g_theGame->ConfirmAttack();
	return true;
}

bool Command_SetTarget(EventArgs& args)
{
	Vec3 hoveringPos = args.GetValue("Pos", Vec3());
	g_theGame->m_hoveringPos = hoveringPos;
	return true;
}

bool Command_PlayerQuit(EventArgs& args)
{
	UNUSED(args);
	g_theGame->m_isOver = true;
	return true;
}

GamePlaying::GamePlaying() : Game()
{

	//m_clock = new Clock(Clock::GetSystemClock());

	//g_theEventSystem->SubscribeEventCallbackFunction("LoadModel", &LoadModel);
	g_theEventSystem->SubscribeEventCallbackFunction("LoadMap", &Command_LoadMap);

	AABB2 bound1;
	bound1.SetCenter(Vec2(g_theApp->m_windowWidth * 0.08f, g_theApp->m_windowHeight * 0.03f));
	bound1.SetDimensions(Vec2(g_theApp->m_windowWidth * 0.155f, g_theApp->m_windowHeight * 0.06f));

	AABB2 bound2;
	bound2.SetCenter(Vec2(g_theApp->m_windowWidth * 0.24f, g_theApp->m_windowHeight * 0.03f));
	bound2.SetDimensions(Vec2(g_theApp->m_windowWidth * 0.155f, g_theApp->m_windowHeight * 0.06f));

	AABB2 bound3;
	bound3.SetCenter(Vec2(g_theApp->m_windowWidth * 0.4f, g_theApp->m_windowHeight * 0.03f));
	bound3.SetDimensions(Vec2(g_theApp->m_windowWidth * 0.155f, g_theApp->m_windowHeight * 0.06f));

	AABB2 bound4;
	bound4.SetCenter(Vec2(g_theApp->m_windowWidth * 0.56f, g_theApp->m_windowHeight * 0.03f));
	bound4.SetDimensions(Vec2(g_theApp->m_windowWidth * 0.155f, g_theApp->m_windowHeight * 0.06f));


	AABB2 bound5;
	bound5.SetCenter(Vec2(g_theApp->m_windowWidth * 0.72f, g_theApp->m_windowHeight * 0.03f));
	bound5.SetDimensions(Vec2(g_theApp->m_windowWidth * 0.155f, g_theApp->m_windowHeight * 0.06f));

	Button* button1 = new Button("[Mouse]", bound1, g_uiFont);
	Button* button2 = new Button("[Left] Previous Unit", bound2, g_uiFont);
	Button* button3 = new Button("[Right] Next Unit", bound3, g_uiFont);
	Button* button4 = new Button("[Escape]", bound4, g_uiFont);
	Button* button5 = new Button("[Enter]", bound5, g_uiFont);

	m_buttons.push_back(button1);
	m_buttons.push_back(button2);
	m_buttons.push_back(button3);
	m_buttons.push_back(button4);
	m_buttons.push_back(button5);

	for (int bIndex = 0; bIndex < (int)m_buttons.size(); bIndex++) {
		m_buttons[bIndex]->m_textCellHeight = 15.f;
		m_buttons[bIndex]->m_alignment = Vec2(0.05f, 0.5f);
		m_buttons[bIndex]->m_normalColor = Rgba8(50, 50, 50);
	}
	LoadAssets();

	g_theEventSystem->SubscribeEventCallbackFunction("PlayerReady", &Command_GetReady);
	g_theEventSystem->SubscribeEventCallbackFunction("StartTurn", &Command_StartTurn);
	g_theEventSystem->SubscribeEventCallbackFunction("SetTarget", &Command_SetTarget);
	g_theEventSystem->SubscribeEventCallbackFunction("Select", &Command_Select);
	g_theEventSystem->SubscribeEventCallbackFunction("SelectPrevious", &Command_SelectLeft);
	g_theEventSystem->SubscribeEventCallbackFunction("SelectNext", &Command_SelectRight);
	g_theEventSystem->SubscribeEventCallbackFunction("Move", &Command_Move);
	g_theEventSystem->SubscribeEventCallbackFunction("ConfirmMove", &Command_ConfirmMove);
	g_theEventSystem->SubscribeEventCallbackFunction("Attack", &Command_Attack);
	g_theEventSystem->SubscribeEventCallbackFunction("ConfirmAttack", &Command_ConfirmAttack);
	g_theEventSystem->SubscribeEventCallbackFunction("Cancel", &Command_Cancel);
	g_theEventSystem->SubscribeEventCallbackFunction("EndTurn", &Command_EndTurn);
	g_theEventSystem->SubscribeEventCallbackFunction("ConfirmEndTurn", &Command_ConfirmEndTurn);
	g_theEventSystem->SubscribeEventCallbackFunction("PlayerQuit", &Command_PlayerQuit);
}

GamePlaying::~GamePlaying()
{
	MapDefinition::ClearDefinitions();
	TileDefinition::ClearDefinitions();
	UnitDefinition::ClearDefinitions();
}

void GamePlaying::Startup()
{
	//LoadAssets();
	
	Vec2 worldCenter(WORLD_CENTER_X, WORLD_CENTER_Y);
	Vec3 playerPos = g_gameConfigBlackboard.GetValue("cameraStartPosition", Vec3(0.f, 0.f, 5.f));
	Vec3 angle = g_gameConfigBlackboard.GetValue("cameraFixedAngle", Vec3(0.f, 0.f, 0.f));
	m_nearClipDist = g_gameConfigBlackboard.GetValue("cameraNearClip", 0.01f);

	std::string mapName = g_gameConfigBlackboard.GetValue("defaultMap", "");
	m_map = new Map(this, mapName.c_str());
	
	for (int playerIndex = 0; playerIndex < 2; playerIndex++) {
		Player* player = new Player(this, playerPos);
		player->m_orientation.m_yawDegrees = angle.x;
		player->m_orientation.m_pitchDegrees = angle.y;
		player->m_orientation.m_rollDegrees = angle.z;
		player->m_camera.m_viewport = AABB2(Vec2(0.f, 0.f),
			Vec2((float)g_theRenderer->GetConfig().m_window->GetClientDimensions().x,
				(float)g_theRenderer->GetConfig().m_window->GetClientDimensions().y));
		player->m_camera.m_perspectiveNear = g_gameConfigBlackboard.GetValue("cameraNearClip", 0.01f);
		player->m_camera.m_perspectiveFar = g_gameConfigBlackboard.GetValue("cameraFarClip", 100.f);
		player->m_camera.m_FOV = g_gameConfigBlackboard.GetValue("cameraFOVDegrees", 100.f);
		player->m_cameraDefaultSpeed = g_gameConfigBlackboard.GetValue("cameraPanSpeed", 2.f);
		player->m_cameraSpeed = player->m_cameraDefaultSpeed;
		m_map->SetUnitsByPlayer(player->m_units, playerIndex + 1);
		m_players.push_back(player);
	}

	if (m_currentPlayerIndex >= 0 && m_currentPlayerIndex < (int)m_players.size()) {
		m_currentPlayer = m_players[m_currentPlayerIndex];
	}

	//std::string filename = g_gameConfigBlackboard.GetValue("defaultModel", "");
	//m_model = new Model(g_gameConfigBlackboard.GetValue("defaultModel", "Data/Models/Cow.xml").c_str());
		//"Data/Models/Teapot.xml");
	m_isOver = false;
	m_isRemoteReady = false;
	SetCamera();

}

void GamePlaying::Update()
{
	if (g_theGameState != GameState::PLAYING) {
		return;
	}
	SetCamera();
	
	
	if (DoesAnyPlayerWin() != -1 || m_isOver) {
		if (g_theInput->WasKeyJustPressed(KEYCODE_LEFT_MOUSE)) {
			Exit();
			g_theGameState = GameState::MAIN;
			g_theGames[(int)g_theGameState]->Enter();
		}
		return;
	}
	UpdateButtonTexts();
	
	m_map->Update(m_clock->GetDeltaSeconds());

	m_map->GeneratePath(GetHoveringPosition());

	if (!m_isPaused)
	{
		UpdateEntities();

		DeleteGarbageEntities();
	}

	UpdateTargetHighlight();

	if (!AreBothReady() || !IsCurrentPlayerLocal()) {
		return;
	}
	Vec2 cursor = g_theWindow->GetNormalizedCursorPos();
	cursor.x *= g_theApp->m_windowWidth;
	cursor.y *= g_theApp->m_windowHeight;

	Vec3 currentHovering = GetHoveringPosition();
	if (GetHoveringTile() != m_map->GetTileByPosition(m_hoveringPos)) {
		m_hoveringPos = currentHovering;
		NetSystem::Event_AddSendMessage(Stringf("SetTarget Pos=%.3f,%.3f,%.3f", m_hoveringPos.x, m_hoveringPos.y, m_hoveringPos.z));
	}

	UpdateFromKeyboard();
	UpdateFromController();

	for (int buttonIndex = 0; buttonIndex < (int)m_buttons.size(); buttonIndex++) {
		m_buttons[buttonIndex]->Update(cursor);
		if (m_buttons[buttonIndex]->m_isHovered) {
			m_focusedButton = m_buttons[buttonIndex];
			m_focusedIndex = buttonIndex;
		}
	}
}

void GamePlaying::Render() const
{
	//if (m_isAttractMode)
	//{
	//	RenderAttractMode();
	//	return;
	//}
	if (g_theGameState != GameState::PLAYING)
		return;
	g_theRenderer->ClearScreen(Rgba8(0, 0, 0));

	if (!AreBothReady()) {
		RenderNetPrompt();
		return;
	}

	//RenderGrid();

	if (IsCurrentPlayerLocal() || m_currentPlayer->m_state != PlayerState::Ready) {
		RenderEntities();
		m_map->Render(m_currentPlayer->m_camera);
		if (m_model != nullptr) {
			m_model->Render();
		}

		if (m_isDebug)
		{
			DebugRenderEntities();
		}
	
		RenderUI();
		RenderSelectedUnit();
	}


	if (m_isPaused)
	{
		RenderPaused();
	}
	
	if (m_currentPlayer->m_state == PlayerState::Ready || DoesAnyPlayerWin() != -1 || m_isOver) {
		RenderPrompt();
	}

	DebugRenderWorld(m_currentPlayer->m_camera);
	DebugRenderScreen(m_screenCamera);
}

void GamePlaying::Shutdown()
{
	Send("PlayerQuit");

	delete m_model;
	m_model = nullptr;
	delete m_map;
	m_map = nullptr;
	delete m_players[0];
	m_players[0] = nullptr;
	delete m_players[1];
	m_players[1] = nullptr;
	m_players.clear();
	m_currentPlayer = nullptr;
	m_currentPlayerIndex = 0;

	delete m_selectedUnit;
	m_selectedUnit = nullptr;
	
	delete m_attackedUnit;
	m_attackedUnit = nullptr;

	m_OGCoords = IntVec2(-1, -1);

}

void GamePlaying::Exit()
{
	g_theInput->EndFrame();
}

void GamePlaying::Enter()
{
	if (g_theNet->GetConfig().m_mode == NetSystemMode::NONE) {
		m_players[0]->m_state = PlayerState::Ready;
		m_players[1]->m_state = PlayerState::Ready;
	}
	else {
		Player* local = GetLocalPlayer();
		Player* remote = GetRemotePlayer();
		local->m_state = PlayerState::Ready;
		g_theNet->Event_AddSendMessage("PlayerReady");
		if (m_isRemoteReady)
			remote->m_state = PlayerState::Ready;
	}
}

void GamePlaying::UpdateEntities()
{

	Mat44 camMat = m_currentPlayer->GetModelMatrix();

	m_currentPlayer->Update();
}

void GamePlaying::LoadAssets()
{
	//g_musicLibrary[MUSIC_WELCOME] = g_theAudio->CreateOrGetSound("Data/Audio/Click.mp3");
	TileDefinition::InitializeTileDefs("Data/Definitions/TileDefinitions.xml");
	UnitDefinition::InitializeUnitDefs("Data/Definitions/UnitDefinitions.xml");
	MapDefinition::InitializeMapDefs("Data/Definitions/MapDefinitions.xml");
	g_textureLibrary[TEXTURE_SPHERE] = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/TestUV.png");
	g_textureLibrary[TEXTURE_CUBE] = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/Test_StbiFlippedAndOpenGL.png");
	g_textureLibrary[TEXTURE_GROUND] = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/MoonSurface.png");
}

void GamePlaying::DeleteGarbageEntities()
{

}

void GamePlaying::KillAll()
{

}

void GamePlaying::UpdateButtonTexts()
{
	if (m_currentPlayer->m_state == PlayerState::Selecting) {
		m_buttons[0]->m_text = "[Mouse] Select";
	}
	if (m_currentPlayer->m_state == PlayerState::UnitSelected) {
		m_buttons[0]->m_text = "[Mouse] Move";
	}
	if (m_currentPlayer->m_state == PlayerState::UnitMoved) {
		m_buttons[0]->m_text = "[Mouse] Confirm Move";
	}
	if (m_currentPlayer->m_state == PlayerState::UnitMoveConfirmed) {
		m_buttons[0]->m_text = "[Mouse] Attack / Done";
	}
	if (m_currentPlayer->m_state == PlayerState::UnitAttacked) {
		m_buttons[0]->m_text = "[Mouse] Confirm Attack";
	}
	if (m_currentPlayer->m_state != PlayerState::EndTurn) {
		m_buttons[4]->m_text = "[Enter] Confirm EndTurn";
	}
	else{
		m_buttons[4]->m_text = "[Enter] EndTurn";
	}

	if (m_currentPlayer->m_state == PlayerState::UnitSelected
		|| m_currentPlayer->m_state == PlayerState::UnitMoved
		|| m_currentPlayer->m_state == PlayerState::UnitAttacked) {
		m_buttons[3]->m_text = "[Escape] Cancel";
	}
	else if (m_currentPlayer->m_state == PlayerState::Selecting ||
		m_currentPlayer->m_state == PlayerState::UnitMoveConfirmed ||
		m_currentPlayer->m_state == PlayerState::EndTurn
		) {
		m_buttons[3]->m_text = "[Escape] Exit";
	}
}

void GamePlaying::UpdateFromKeyboard()
{
	
	if (g_theInput->WasKeyJustPressed(KEYCODE_F1))
	{
		m_isDebug = !m_isDebug;
	}
	if (g_theInput->WasKeyJustPressed('L')) {
		g_theInput->SetCursorMode(false, false);

		std::string filename;
		filename = g_theWindow->FileOpenDialog();
		
		if (IsFileExist(filename)) {
			if (m_model) {
				delete m_model;
				m_model = nullptr;
			}
			m_model = new Model(filename.c_str());
		}

		g_theInput->SetCursorMode(true, true);
		EventArgs args;
		args.SetValue("KeyCode", Stringf("%d", (unsigned char)'L'));
		g_theEventSystem->FireEvent("KeyReleased", args);

	}
	if (g_theInput->WasKeyJustPressed(KEYCODE_LEFT_MOUSE)) {
		if (m_currentPlayer->m_state == PlayerState::Ready) {
			StartTurn();
		}
		else if (!GetHoveringTile()) {
			// click on somewhere else doesn't count click
		}
		else if (m_currentPlayer->m_state == PlayerState::Selecting) {
			Select();
		}
		else if (m_currentPlayer->m_state == PlayerState::UnitSelected) {
			Select();
		}
		else if (m_currentPlayer->m_state == PlayerState::UnitMoved) {
			ConfirmMove();
		}
		else if (m_currentPlayer->m_state == PlayerState::UnitMoveConfirmed) {
			Attack();
		}
		else if (m_currentPlayer->m_state == PlayerState::UnitAttacked) {
			ConfirmAttack();
		}
	}
	if (m_buttons[1]->m_isSelected || g_theInput->WasKeyJustPressed(KEYCODE_LEFTARROW)) {
		if (m_currentPlayer->m_state == PlayerState::Selecting) {
			SelectLeft();
		}
		else if (m_currentPlayer->m_state == PlayerState::UnitSelected) {
			SelectLeft();
		}
	}
	if (m_buttons[2]->m_isSelected || g_theInput->WasKeyJustPressed(KEYCODE_RIGHTARROW)) {
		if (m_currentPlayer->m_state == PlayerState::Selecting) {
			SelectRight();
		}
		else if (m_currentPlayer->m_state == PlayerState::UnitSelected) {
			SelectRight();
		}
	}
	if (m_buttons[4]->m_isSelected) {
		if (m_currentPlayer->m_state == PlayerState::Selecting) {
			EndTurn();
		}
		else if (m_currentPlayer->m_state == PlayerState::EndTurn) {
			ConfirmEndTurn();
		}
	}
	if (m_buttons[3]->m_isSelected) {
		if (m_currentPlayer->m_state == PlayerState::UnitSelected
			|| m_currentPlayer->m_state == PlayerState::UnitMoved
			|| m_currentPlayer->m_state == PlayerState::UnitAttacked) {
			Cancel();
		}
		else if (m_currentPlayer->m_state == PlayerState::Selecting ||
			m_currentPlayer->m_state == PlayerState::UnitMoveConfirmed
			) {
			Exit();
			g_theGameState = GameState::PAUSE;
		}
		else if (m_currentPlayer->m_state == PlayerState::EndTurn) {
			m_currentPlayer->m_state = PlayerState::Selecting;
		}
	}
	if (g_theInput->WasKeyJustPressed(KEYCODE_ENTER)) {
		if (m_currentPlayer->m_state == PlayerState::Ready) {
			StartTurn();	
		}
		else if (m_currentPlayer->m_state == PlayerState::Selecting) {
			EndTurn();
		}
		else if (m_currentPlayer->m_state == PlayerState::EndTurn) {
			ConfirmEndTurn();
		}
	}
	if (g_theInput->WasKeyJustPressed(KEYCODE_ESC)) {
		if (m_currentPlayer->m_state == PlayerState::UnitSelected 
			|| m_currentPlayer->m_state == PlayerState::UnitMoved
			|| m_currentPlayer->m_state == PlayerState::UnitAttacked) {
			Cancel();
		}
		else if (m_currentPlayer->m_state == PlayerState::Selecting ||
			m_currentPlayer->m_state == PlayerState::UnitMoveConfirmed
			) {
			Exit();
			g_theGameState = GameState::PAUSE;
		}
		else if (m_currentPlayer->m_state == PlayerState::EndTurn) {
			m_currentPlayer->m_state = PlayerState::Selecting;
		}
	}
}

void GamePlaying::UpdateTargetHighlight()
{
	if (!m_selectedUnit) {
		int playerIndex = 1;
		if (m_currentPlayerIndex == 1)
			playerIndex = 0;
		Player* waiting = m_players[playerIndex];
		for (int unitIndex = 0; unitIndex < (int)waiting->m_units.size(); unitIndex++) {
			Unit*& unit = waiting->m_units[unitIndex];
			if (unit->m_state == UnitState::Targeted)
				unit->m_state = UnitState::Ready;
		}
		return;
	}
	int playerIndex = 1;
	if (m_currentPlayerIndex == 1)
		playerIndex = 0;
	Player* waiting = m_players[playerIndex];
	for (int unitIndex = 0; unitIndex < (int)waiting->m_units.size(); unitIndex++) {
		Unit*& unit = waiting->m_units[unitIndex];
		if (m_selectedUnit->IsCoordsInAttackRange(unit->m_coords) 
			&& !m_attackedUnit && unit->m_state != UnitState::Dead
			) {
			unit->m_state = UnitState::Targeted;
		}
		else {
			if (unit->m_state == UnitState::Targeted)
			unit->m_state = UnitState::Ready;
		}
	}
}

void GamePlaying::UpdateFromController()
{

}

void GamePlaying::RenderAttractMode() const
{
	g_theRenderer->BeginCamera(m_screenCamera);
	
	g_theRenderer->EndCamera(m_screenCamera);
}

void GamePlaying::RenderUI() const
{
	g_theRenderer->BeginCamera(m_screenCamera);

	DrawSquare(Vec2(g_theApp->m_windowWidth * 0.80f, 0.f), Vec2(g_theApp->m_windowWidth, g_theApp->m_windowHeight * 0.36f), Rgba8::BLACK);
	DrawSquare(Vec2(g_theApp->m_windowWidth * 0.80f, 0.f), Vec2(g_theApp->m_windowWidth * 0.805f, g_theApp->m_windowHeight * 0.36f), Rgba8::GREY);
	DrawSquare(Vec2(g_theApp->m_windowWidth * 0.995f, 0.f), Vec2(g_theApp->m_windowWidth, g_theApp->m_windowHeight * 0.36f), Rgba8::GREY);
	DrawSquare(Vec2(g_theApp->m_windowWidth * 0.80f, 0.f), Vec2(g_theApp->m_windowWidth, g_theApp->m_windowHeight * 0.0025f), Rgba8::GREY);
	DrawSquare(Vec2(g_theApp->m_windowWidth * 0.80f, g_theApp->m_windowHeight * 0.0575f), Vec2(g_theApp->m_windowWidth, g_theApp->m_windowHeight * 0.0625f), Rgba8::GREY);
	DrawSquare(Vec2(g_theApp->m_windowWidth * 0.80f, g_theApp->m_windowHeight * 0.1175f), Vec2(g_theApp->m_windowWidth, g_theApp->m_windowHeight * 0.1225f), Rgba8::GREY);
	DrawSquare(Vec2(g_theApp->m_windowWidth * 0.80f, g_theApp->m_windowHeight * 0.1775f), Vec2(g_theApp->m_windowWidth, g_theApp->m_windowHeight * 0.1825f), Rgba8::GREY);
	DrawSquare(Vec2(g_theApp->m_windowWidth * 0.80f, g_theApp->m_windowHeight * 0.2375f), Vec2(g_theApp->m_windowWidth, g_theApp->m_windowHeight * 0.2425f), Rgba8::GREY);
	DrawSquare(Vec2(g_theApp->m_windowWidth * 0.80f, g_theApp->m_windowHeight * 0.2975f), Vec2(g_theApp->m_windowWidth, g_theApp->m_windowHeight * 0.3025f), Rgba8::GREY);
	DrawSquare(Vec2(g_theApp->m_windowWidth * 0.80f, g_theApp->m_windowHeight * 0.3575f), Vec2(g_theApp->m_windowWidth, g_theApp->m_windowHeight * 0.36f), Rgba8::GREY);

	AABB2 box1 = AABB2(Vec2(g_theApp->m_windowWidth * 0.80f, g_theApp->m_windowHeight * 0.3f), Vec2(g_theApp->m_windowWidth, g_theApp->m_windowHeight * 0.36f));
	AABB2 box2 = AABB2(Vec2(g_theApp->m_windowWidth * 0.80f, g_theApp->m_windowHeight * 0.24f), Vec2(g_theApp->m_windowWidth, g_theApp->m_windowHeight * 0.3f));
	AABB2 box3 = AABB2(Vec2(g_theApp->m_windowWidth * 0.80f, g_theApp->m_windowHeight * 0.18f), Vec2(g_theApp->m_windowWidth, g_theApp->m_windowHeight * 0.24f));
	AABB2 box4 = AABB2(Vec2(g_theApp->m_windowWidth * 0.80f, g_theApp->m_windowHeight * 0.12f),  Vec2(g_theApp->m_windowWidth, g_theApp->m_windowHeight * 0.18f));
	AABB2 box5 = AABB2(Vec2(g_theApp->m_windowWidth * 0.80f, g_theApp->m_windowHeight * 0.06f),  Vec2(g_theApp->m_windowWidth, g_theApp->m_windowHeight * 0.12f));
	AABB2 box6 = AABB2(Vec2(g_theApp->m_windowWidth * 0.80f, 0.f),   Vec2(g_theApp->m_windowWidth, g_theApp->m_windowHeight * 0.06f));

	std::vector<Vertex_PCU> text;
	g_uiFont->AddVertsForTextInBox2D(text, box2, 15.f, "Attack", Rgba8::WHITE, 1.f, Vec2(0.05f, 0.5f));
	g_uiFont->AddVertsForTextInBox2D(text, box3, 15.f, "Defense", Rgba8::WHITE, 1.f, Vec2(0.05f, 0.5f));
	g_uiFont->AddVertsForTextInBox2D(text, box4, 15.f, "Range", Rgba8::WHITE, 1.f, Vec2(0.05f, 0.5f));
	g_uiFont->AddVertsForTextInBox2D(text, box5, 15.f, "Move", Rgba8::WHITE, 1.f, Vec2(0.05f, 0.5f));
	g_uiFont->AddVertsForTextInBox2D(text, box6, 15.f, "Health", Rgba8::WHITE, 1.f, Vec2(0.05f, 0.5f));

	Unit* unit = GetHoveringUnit();
	if (unit) {
		std::string tank = "T";
		std::string artilery = "A";
		std::string type;
		if (unit->m_unitDef->m_type == UnitType::Tank)
			type = tank;
		else type = artilery;
		g_uiFont->AddVertsForTextInBox2D(text, box1, 15.f, Stringf("%s - %s", unit->m_unitDef->m_name.c_str(), type.c_str()), Rgba8::WHITE, 1.f);
		g_uiFont->AddVertsForTextInBox2D(text, box2, 15.f, Stringf("%.0f", unit->m_unitDef->m_groundAttackDamage), Rgba8::WHITE, 1.f, Vec2(0.95f, 0.5f));
		g_uiFont->AddVertsForTextInBox2D(text, box3, 15.f, Stringf("%.0f", unit->m_unitDef->m_groundDefenese), Rgba8::WHITE, 1.f, Vec2(0.95f, 0.5f));
		g_uiFont->AddVertsForTextInBox2D(text, box4, 15.f, Stringf("%.0f - %.0f", unit->m_unitDef->m_groundAttackRange_Min, unit->m_unitDef->m_groundAttackRange_Max), Rgba8::WHITE, 1.f, Vec2(0.95f, 0.5f));
		g_uiFont->AddVertsForTextInBox2D(text, box5, 15.f, Stringf("%.0f", unit->m_unitDef->m_movementRange), Rgba8::WHITE, 1.f, Vec2(0.95f, 0.5f));
		g_uiFont->AddVertsForTextInBox2D(text, box6, 15.f, Stringf("%.0f", unit->m_currentHealth), Rgba8::WHITE, 1.f, Vec2(0.95f, 0.5f));
	}

	for (int buttonIndex = 0; buttonIndex < (int)m_buttons.size(); buttonIndex++) {
		m_buttons[buttonIndex]->Render();
	}

	g_theRenderer->BindTexture(&g_uiFont->GetTexture());
	g_theRenderer->DrawVertexArray((int)text.size(), text.data());
	g_theRenderer->BindTexture(nullptr);

	g_theRenderer->EndCamera(m_screenCamera);
}


void GamePlaying::RenderPrompt() const
{
	g_theRenderer->BeginCamera(m_screenCamera);

	AABB2 box1, box2;
	box1.SetCenter(g_theApp->m_windowDimension * 0.5f);
	box1.SetDimensions(g_theApp->m_windowDimension * 0.4f);
	box2.SetCenter(g_theApp->m_windowDimension * 0.5f);
	box2.SetDimensions(g_theApp->m_windowDimension * 0.39f);

	DrawSquare(box1.m_mins, box1.m_maxs, Rgba8::WHITE);
	DrawSquare(box2.m_mins, box2.m_maxs, Rgba8::BLACK);
	std::vector<Vertex_PCU> text;
	AABB2 player = box1;
	player.SetCenter(Vec2(g_theApp->m_windowWidth * 0.5f, g_theApp->m_windowHeight * 0.6f));
	player.SetDimensions(g_theApp->m_windowDimension * 0.35f);
	AABB2 click = box2;
	click.SetCenter(Vec2(g_theApp->m_windowWidth * 0.5f, g_theApp->m_windowHeight * 0.4f));
	click.SetDimensions(g_theApp->m_windowDimension * 0.35f);
	int win = DoesAnyPlayerWin();
	if (win == -1) {
		g_uiFont->AddVertsForTextInBox2D(text, player, 50.f, Stringf("Player %d's Turn", m_currentPlayerIndex + 1));
		if (IsCurrentPlayerLocal())
			g_uiFont->AddVertsForTextInBox2D(text, click, 20.f, Stringf("Press Enter or Click to Start", m_currentPlayerIndex + 1));
	}
	else {
		if (win != 2) {
			g_uiFont->AddVertsForTextInBox2D(text, player, 50.f, Stringf("Player %d Wins", win + 1));
		}
		else {
			g_uiFont->AddVertsForTextInBox2D(text, player, 50.f, Stringf("Players Tied"));
		}
		g_uiFont->AddVertsForTextInBox2D(text, click, 20.f, Stringf("Press Enter or Click to Start", m_currentPlayerIndex + 1));
	}

	g_theRenderer->BindTexture(&g_uiFont->GetTexture());
	g_theRenderer->DrawVertexArray((int)text.size(), text.data());
	g_theRenderer->BindTexture(nullptr);
	g_theRenderer->EndCamera(m_screenCamera);

}

void GamePlaying::RenderNetPrompt() const
{
	g_theRenderer->BeginCamera(m_screenCamera);

	g_theRenderer->ClearScreen(Rgba8::BLACK);
	AABB2 box1, box2;
	box1.SetCenter(g_theApp->m_windowDimension * 0.5f);
	box1.SetDimensions(g_theApp->m_windowDimension * 0.9f);
	box2.SetCenter(g_theApp->m_windowDimension * 0.5f);
	box2.SetDimensions(g_theApp->m_windowDimension * 0.85f);

	DrawSquare(box1.m_mins, box1.m_maxs, Rgba8::WHITE);
	DrawSquare(box2.m_mins, box2.m_maxs, Rgba8::BLACK);
	std::vector<Vertex_PCU> text;
	AABB2 player = box1;
	player.SetCenter(Vec2(g_theApp->m_windowWidth * 0.5f, g_theApp->m_windowHeight * 0.6f));
	player.SetDimensions(g_theApp->m_windowDimension * 0.35f);
	AABB2 click = box2;
	click.SetCenter(Vec2(g_theApp->m_windowWidth * 0.5f, g_theApp->m_windowHeight * 0.4f));
	click.SetDimensions(g_theApp->m_windowDimension * 0.35f);
	g_uiFont->AddVertsForTextInBox2D(text, player, 50.f, Stringf("Waiting for player..."), Rgba8::WHITE, 1.0f, Vec2(0.5f, 0.8f));

	g_theRenderer->BindTexture(&g_uiFont->GetTexture());
	g_theRenderer->DrawVertexArray((int)text.size(), text.data());
	g_theRenderer->BindTexture(nullptr);
	g_theRenderer->EndCamera(m_screenCamera);
}

void GamePlaying::RenderEntities() const
{
	g_theRenderer->BeginCamera(m_currentPlayer->m_camera);

	std::vector<Vertex_PCU> ground;

	AddVertsForQuad3D(ground, Vec3(-50.f, -50.f, -0.001f), Vec3(50.f, -50.f, -0.001f), Vec3(50.f, 50.f, -0.001f), Vec3(-50.f, 50.f, -0.001f), Rgba8::WHITE);

	g_theRenderer->SetModelConstants();
	g_theRenderer->BindShader(nullptr);
	g_theRenderer->BindTexture(g_textureLibrary[TEXTURE_GROUND]);

	g_theRenderer->DrawVertexArray((int)ground.size(), ground.data());

	g_theRenderer->BindTexture(nullptr);

	for (int playerIndex = 0; playerIndex < 2; playerIndex++) {
		for (int unitIndex = 0; unitIndex < (int)m_players[playerIndex]->m_units.size(); unitIndex++) {
			const Unit* unit = m_players[playerIndex]->m_units[unitIndex];
			unit->Render();
			unit->RenderSelected();
		}
	}

	g_theRenderer->EndCamera(m_currentPlayer->m_camera);
}

void GamePlaying::RenderPaused() const
{
	g_theRenderer->BeginCamera(m_screenCamera);
	DrawSquare(m_screenCamera.m_bottomLeft, m_screenCamera.m_topRight, Rgba8(127, 127, 127, 127));
	g_theRenderer->EndCamera(m_screenCamera);
}

void GamePlaying::RenderGrid() const
{
	g_theRenderer->BeginCamera(m_currentPlayer->m_camera);

	g_theRenderer->SetModelConstants();
	g_theRenderer->BindTexture(nullptr);
	std::vector<Vertex_PCU> grids;
	for (int xIndex = -50; xIndex <= 50; xIndex++) {
		float offset = 0.02f;
		Rgba8 color = Rgba8::GREY;
		if (xIndex % 5 == 0) {
			color = Rgba8::GREEN;
			offset *= 2;
		}
		Vec2 mins = Vec2(float(xIndex) - offset, -50.f);
		Vec2 maxs = Vec2(float(xIndex) + offset, 50.f);
		AddVertsForAABB2D(grids, AABB2(mins, maxs), color, Vec2(0.f, 0.f), Vec2(1.f, 1.f));
	}

	for (int yIndex = -50; yIndex <= 50; yIndex++) {
		float offset = 0.02f;
		Rgba8 color = Rgba8::GREY;
		if (yIndex % 5 == 0) {
			color = Rgba8::RED;
			offset *= 2;
		}
		Vec2 mins = Vec2(-50.f, float(yIndex) - offset);
		Vec2 maxs = Vec2(50.f, float(yIndex) + offset);
		AddVertsForAABB2D(grids, AABB2(mins, maxs), color, Vec2(0.f, 0.f), Vec2(1.f, 1.f));
	}
	g_theRenderer->DrawVertexArray((int)grids.size(), grids.data());
	g_theRenderer->EndCamera(m_currentPlayer->m_camera);
}

void GamePlaying::RenderSelectedUnit() const
{
	g_theRenderer->BeginCamera(m_currentPlayer->m_camera);
	//g_theRenderer->BindShader(nullptr);
	std::vector<Vertex_PCU> frame;
	Tile* tile = GetHoveringTile();
	Unit* unit = GetHoveringUnit();
	if (tile) {
		Vec3 center = tile->m_center;
		center.z = 0.03f;
		if (unit) {
		
			if (!m_currentPlayer->IsUnitMine(unit)) {
				AddVertsForHexgon3DFrame(frame, 0.5f, center, 0.03f, Rgba8::RED);
			}
			else {
				AddVertsForHexgon3DFrame(frame, 0.5f, center, 0.03f, Rgba8::BLUE);
			}
		}
		else
			AddVertsForHexgon3DFrame(frame, 0.5f, center, 0.03f, Rgba8::GREEN);
		Mat44 mat = GetBillboardMatrix(BillboardType::FULL_CAMERA_FACING, m_currentPlayer->m_camera.GetModelMatrix(), tile->m_center);
		g_uiFont->AddVertsForText3D(frame, Vec2(0.f, 0.f), 0.9f,
			Stringf("%d, %d", tile->m_tileCoords.x, tile->m_tileCoords.y), Rgba8::RED, mat);
	}
	//AddVertsForHexgon3DFrame(unit, 0.4f, GetHoveringPosition(), 0.03f, Rgba8::BLUE);
	g_theRenderer->DrawVertexArray((int)frame.size(), frame.data());


	g_theRenderer->EndCamera(m_currentPlayer->m_camera);
}



void GamePlaying::DebugRenderEntities() const
{
	g_theRenderer->BeginCamera(m_currentPlayer->m_camera);

	g_theRenderer->EndCamera(m_currentPlayer->m_camera);
}

void GamePlaying::EnterGame()
{
	//SoundID testSound = g_theAudio->CreateOrGetSound("Data/Audio/Click.mp3");
	//PlayMusic(MUSIC_WELCOME);
	//g_theAudio->StartSound(testSound);
	Shutdown();
	Startup();
	m_isAttractMode = false;
	g_theInput->EndFrame();
}

void GamePlaying::SetCamera()
{
	//m_worldCamera.SetOrthoView(Vec2(-1.f, -1.f), Vec2(WORLD_CAMERA_SIZE_X, WORLD_CAMERA_SIZE_Y));
	m_currentPlayer->m_camera.SetPerspectiveView(m_currentPlayer->m_camera.m_aspect, 
		m_currentPlayer->m_camera.m_FOV, m_currentPlayer->m_camera.m_perspectiveNear, m_currentPlayer->m_camera.m_perspectiveFar);
	m_currentPlayer->m_camera.SetRenderBasis(Vec3(0.f, 0.f, 1.f), Vec3(-1.f, 0.f, 0.f), Vec3(0.f, 1.f, 0.f));
	m_screenCamera.SetOrthoView(Vec2(0.f, 0.f), Vec2(g_theApp->m_windowWidth, g_theApp->m_windowHeight));
}

Unit* GamePlaying::GetUnitByPosition(const Vec3& position) const
{
	for (int playerIndex = 0; playerIndex < 2; playerIndex++) {
		for (int unitIndex = 0; unitIndex < (int)m_players[playerIndex]->m_units.size(); unitIndex++) {
			Unit* unit = m_players[playerIndex]->m_units[unitIndex];
			if (unit && unit->IsPointInside(position) && unit->m_state != UnitState::Dead) {
				return unit;
			}
		}
	}
	return nullptr;
}

Tile* GamePlaying::GetTileByPosition(const Vec3& position) const
{
	return m_map->GetTileByPosition(position);
}

Unit* GamePlaying::GetHoveringUnit() const
{
	Vec3 pos = GetHoveringPosition();
	return GetUnitByPosition(pos);
}

Tile* GamePlaying::GetHoveringTile() const
{
	Vec3 pos = GetHoveringPosition();
	return GetTileByPosition(pos);
}

Player* GamePlaying::GetWaitingPlayer() const
{
	int playerIndex = 1;
	if (m_currentPlayerIndex == 1)
		playerIndex = 0;
	Player* waiting = m_players[playerIndex];
	return waiting;
}


Player* GamePlaying::GetLocalPlayer() const
{
	if (g_theNet->GetConfig().m_mode == NetSystemMode::SERVER) {
		return m_players[0];
	}
	if (g_theNet->GetConfig().m_mode == NetSystemMode::CLIENT) {
		return m_players[1];
	}
	if (g_theNet->GetConfig().m_mode == NetSystemMode::NONE) {
		return m_currentPlayer;
	}
	return nullptr;
}

Player* GamePlaying::GetRemotePlayer() const
{
	if (g_theNet->GetConfig().m_mode == NetSystemMode::SERVER) {
		return m_players[1];
	}
	if (g_theNet->GetConfig().m_mode == NetSystemMode::CLIENT) {
		return m_players[0];
	}
	if (g_theNet->GetConfig().m_mode == NetSystemMode::NONE) {
		return m_currentPlayer;
	}
	return nullptr;
}

bool GamePlaying::IsCurrentPlayerLocal() const
{
	if (m_currentPlayer == GetLocalPlayer())
		return true;
	return false;
}

bool GamePlaying::IsConnected() const
{
	if (g_theNet->GetConfig().m_mode == NetSystemMode::NONE)
		return true;
	else return g_theNet->AmIConnected();
}

bool GamePlaying::AreBothReady() const
{
	if (m_players[0]->m_state == PlayerState::Connected || m_players[1]->m_state == PlayerState::Connected)
		return false;
	return true;
}

Vec3 GamePlaying::CalcuteMouseRayDirection() const
{
	Vec3 forward, left, up;
	m_currentPlayer->m_camera.m_orientation.GetVectors_XFwd_YLeft_ZUp(forward, left, up);

	float height = m_currentPlayer->m_camera.m_perspectiveNear * TanDegrees(m_currentPlayer->m_camera.m_FOV * 0.5f) * 2.f;
	float width = m_currentPlayer->m_camera.m_aspect * height;
	Vec2 mouseScreen = g_theInput->GetCursorNormalizedPosition();
	Vec3 dx = width * (mouseScreen.x - 0.5f) * -left;
	Vec3 dy = height * (mouseScreen.y - 0.5f) * up;
	Vec3 fd = m_currentPlayer->m_camera.m_perspectiveNear * forward;

	//Vec3 point = dx + dy + fd + m_player->m_position;
	Vec3 direction = dx + dy + fd;
	return direction.GetNormalized();
	//Vec2 mouseScreenPos = 
}

IntVec2 GamePlaying::GetHoveringCoords() const
{
	return m_map->GetCoordByPosition(GetHoveringPosition());
}

Vec3 GamePlaying::GetHoveringPosition() const
{
	if (IsCurrentPlayerLocal()) {
		Vec3 direction = CalcuteMouseRayDirection();
		RaycastResult3D result = m_map->RaycastWorldZ(m_currentPlayer->m_position, direction);
		std::vector<unsigned int> hitIndex;
		if (result.m_didImpact) {
			Vec3 hitPos = result.m_impactPos;
			
			return hitPos;
			//DrawCircle(hitPos.GetVec2XY(), 0.1f, Rgba8::RED);
		}
		return Vec3();
	}
	else {
		return m_hoveringPos;
	}
}

void GamePlaying::StartTurn()
{
	m_currentPlayer->StartTurn();
	Send("StartTurn");
}

void GamePlaying::EndTurn()
{
	m_currentPlayer->EndTurn();
	Send("EndTurn");
}

void GamePlaying::ConfirmEndTurn()
{
	Send("ConfirmEndTurn");
	m_currentPlayer->ConfirmEndTurn();
	for (int unitIndex = 0; unitIndex < (int)m_currentPlayer->m_units.size(); unitIndex++) {
		Unit*& unit = m_currentPlayer->m_units[unitIndex];
		// #CheckDeath
		if (unit->m_state != UnitState::Dead) {
			unit->m_state = UnitState::Ready;
		}
	}
	if (m_currentPlayer == m_players[0]) {
		m_currentPlayer = m_players[1];
		m_currentPlayerIndex = 1;
	}
	else {
		m_currentPlayer = m_players[0];
		m_currentPlayerIndex = 0;
	}
	m_currentPlayer->m_state = PlayerState::Ready;
}

void GamePlaying::Select()
{
	Send("Select");
	Unit* hoveredUnit = GetHoveringUnit();
	if (hoveredUnit != nullptr)
	{
		if (m_currentPlayer->IsUnitMine(hoveredUnit)) {
			if (hoveredUnit->m_state == UnitState::Ready) {
				if (m_selectedUnit) {
					m_selectedUnit->m_state = UnitState::Ready;
				}
				m_selectedUnit = hoveredUnit;
				m_OGCoords = m_selectedUnit->m_coords;
				m_selectedUnit->m_state = UnitState::Selected;
				m_map->GenerateDistanceField(GetHoveringCoords(), (int)m_selectedUnit->m_unitDef->m_movementRange);
				m_currentPlayer->Select();
			}
		}
		else if (m_selectedUnit){
			Attack();
		}
		return;
	}
	Tile* hoveredTile = GetHoveringTile();
	if (hoveredTile != nullptr && m_map->m_defaultNodes[hoveredTile->m_tileCoords].m_distance < 9999999) {
		if (m_selectedUnit) {
			if (hoveredTile->m_tileCoords != m_selectedUnit->m_coords) {
				m_selectedUnit->m_position = hoveredTile->m_center;
				m_selectedUnit->m_coords = hoveredTile->m_tileCoords;
				m_currentPlayer->Move();
			}
		}
	}

}

void GamePlaying::SelectLeft()
{
	Send("SelectPrevious");
	if (m_selectedUnit) {
		if (m_selectedUnit) {
			m_selectedUnit->m_state = UnitState::Ready;
		}
		int index = 0;
		for (int uIndex = 0; uIndex < (int)m_currentPlayer->m_units.size(); uIndex++) {
			Unit* unit = m_currentPlayer->m_units[uIndex];
			if (unit == m_selectedUnit) {
				index = uIndex;
				break;
			}
		}
		while (1) {
			if (index == 0)
				index = (int)m_currentPlayer->m_units.size() - 1;
			else index--;
			if (m_currentPlayer->m_units[index]->m_state != UnitState::Dead && 
				m_currentPlayer->m_units[index]->m_state != UnitState::Finished) {
				m_selectedUnit = m_currentPlayer->m_units[index];
				break;
			}
		}
	}
	else {
		m_selectedUnit = m_currentPlayer->m_units[0];
	}
	m_OGCoords = m_selectedUnit->m_coords;
	m_selectedUnit->m_state = UnitState::Selected;
	m_map->GenerateDistanceField(m_selectedUnit->m_coords, (int)m_selectedUnit->m_unitDef->m_movementRange);
	m_currentPlayer->Select();
}

void GamePlaying::SelectRight()
{
	Send("SelectNext");
	if (m_selectedUnit) {
		if (m_selectedUnit) {
			m_selectedUnit->m_state = UnitState::Ready;
		}
		int index = 0;
		for (int uIndex = 0; uIndex < (int)m_currentPlayer->m_units.size(); uIndex++) {
			Unit* unit = m_currentPlayer->m_units[uIndex];
			if (unit == m_selectedUnit) {
				index = uIndex;
				break;
			}
		}
		while (1) {
			if (index == (int)m_currentPlayer->m_units.size() - 1)
				index = 0;
			else index++;
			if (m_currentPlayer->m_units[index]->m_state != UnitState::Dead &&
				m_currentPlayer->m_units[index]->m_state != UnitState::Finished) {
				m_selectedUnit = m_currentPlayer->m_units[index];
				break;
			}
		}
	}
	else {
		m_selectedUnit = m_currentPlayer->m_units[(int)m_currentPlayer->m_units.size() - 1];
	}
	m_OGCoords = m_selectedUnit->m_coords;
	m_selectedUnit->m_state = UnitState::Selected;
	m_map->GenerateDistanceField(m_selectedUnit->m_coords, (int)m_selectedUnit->m_unitDef->m_movementRange);
	m_currentPlayer->Select();
}

void GamePlaying::Cancel()
{
	Send("Cancel");
	if (m_selectedUnit) {
		m_selectedUnit->m_position = m_map->GetPostionByCoords(m_OGCoords);
		m_selectedUnit->m_coords = m_OGCoords;
		m_OGCoords = IntVec2(-1, -1);
		m_selectedUnit->m_state = UnitState::Ready;
		m_selectedUnit = nullptr;
	}
	m_map->ResetDefaultField();
	m_currentPlayer->Cancel();
}

void GamePlaying::Move()
{
	Send("Move");
	m_currentPlayer->Move();
}

void GamePlaying::ConfirmMove()
{
	Send("ConfirmMove");
	Unit* unit = GetHoveringUnit();
	if (unit == m_selectedUnit && unit != nullptr) {
		m_selectedUnit->m_state = UnitState::Moved;
		m_map->ResetDefaultField();
		m_currentPlayer->ConfirmMove();
		if (m_selectedUnit->m_unitDef->m_type == UnitType::Artillery) {
			ConfirmAttack();
		}
	}
}

void GamePlaying::Attack()
{
	Send("Attack");
	Unit* hoveredUnit = GetHoveringUnit();
	if (hoveredUnit) {
		if (!m_currentPlayer->IsUnitMine(hoveredUnit)) {
			if (m_selectedUnit->IsCoordsInAttackRange(hoveredUnit->m_coords)) {
				m_attackedUnit = hoveredUnit;
				m_attackedUnit->m_state = UnitState::Damaged;
				m_currentPlayer->Attack();
				return;
			}
		}
	}
	ConfirmAttack();
}

void GamePlaying::ConfirmAttack()
{
	Send("ConfirmAttack");
	if (m_attackedUnit)
	{
		Unit* hoveredUnit = GetHoveringUnit();
		if (hoveredUnit == m_attackedUnit)
		{
			m_attackedUnit->TakeDamage(m_selectedUnit->m_unitDef->m_groundAttackDamage);
			if (m_attackedUnit->m_state != UnitState::Dead)
			m_attackedUnit->m_state = UnitState::Ready;
			if (m_attackedUnit->IsCoordsInAttackRange(m_selectedUnit->m_coords)) {
				m_selectedUnit->TakeDamage(m_attackedUnit->m_unitDef->m_groundAttackDamage);
			}
			m_attackedUnit = nullptr;
		}
		else return;
	}
	if (m_selectedUnit) {
		if (m_selectedUnit->m_state != UnitState::Dead)
			m_selectedUnit->m_state = UnitState::Finished;
		m_OGCoords = IntVec2(-1, -1);
		m_selectedUnit = nullptr;
		m_currentPlayer->ConfirmAttack();
		m_map->ResetDefaultField();
	}
}

void GamePlaying::SetCurUnit(IntVec2 currentCoords)
{
	m_selectCoords = currentCoords;
}

void GamePlaying::SetTarget(IntVec2 targetCoords)
{
	m_targetCoords = targetCoords;
}

void GamePlaying::Send(std::string msg)
{
	if (IsCurrentPlayerLocal() && g_theNet->GetConfig().m_mode != NetSystemMode::NONE)
		NetSystem::Event_AddSendMessage(msg);
}

int GamePlaying::DoesAnyPlayerWin() const
{
	int res = -1;
	for (int playerIndex = 0; playerIndex < (int)m_players.size(); playerIndex++) {
		bool flag = true;
		for (int unitIndex = 0; unitIndex < (int)m_players[playerIndex]->m_units.size(); unitIndex++) {
			if (m_players[playerIndex]->m_units[unitIndex]->m_state != UnitState::Dead) {
				flag = false;
				break;
			}
		}
		if (flag) {
			res = playerIndex;
			break;
		}
	}
	if (res == 0)
		return 1;
	if (res == 1)
		return 0;
	if (m_isOver)
		return 2;
	return res;
}
