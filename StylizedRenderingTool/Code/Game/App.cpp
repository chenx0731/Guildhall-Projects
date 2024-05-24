#define UNUSED(x) (void)(x);
#include "Game/App.hpp"
#include "Game/GameCommon.hpp"
//#include "Game/SRTEditor.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Core/DevConsole.hpp"
#include "Engine/Core/DebugRenderSystem.hpp"
//#include "Engine/UI/ImGuiBase.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Renderer/Shader.hpp"
#include "Engine/Renderer/ShaderManager.hpp"

Game*					g_theGame = nullptr;
Renderer*				g_theRenderer = nullptr;
InputSystem*			g_theInput = nullptr;
RandomNumberGenerator*	g_rng = nullptr;
AudioSystem*			g_theAudio = nullptr;
Window*					g_theWindow = nullptr;
DevConsole*				g_theDevConsole = nullptr;
GameStatus				g_theGameStatus;
ImGuiBase*				g_ImGui = nullptr;
ShaderManager*			g_shaderManager = nullptr;

SoundID					g_musicLibrary[MUSIC_NUM];
SoundID					g_lastPlayedMusic[MUSIC_NUM];
Texture*				g_textureLibrary[TEXTURE_NUM];


Texture*				g_filterTexture = nullptr;

Shader*					g_kuwaharaShader = nullptr;

Shader*					g_copyShader = nullptr;

void ShowAllKeysOnDevConsole();

App::App()
{
	m_devCamera = new Camera();
	m_devCamera->m_bottomLeft = Vec2(0.f, 0.f);
	m_devCamera->m_topRight = Vec2(1600.f, 800.f);
}

App::~App()
{

}

bool Command_Quit(EventArgs& args) // Note: STATIC is #define’d to be nothing; just a marker for the reader
{
	UNUSED(args);
	//DebuggerPrintf("Test command received\n");
	g_theApp->HandleQuitRequested();
	//args.DebugPrintContents(); // This method (NamedStrings::DebugPrintContents) is not required; it prints out all key/value pairs
	return true; // Does not consume the event; continue to call other subscribers’ callback functions
}

void App::Startup()
{	
	EventSystemConfig eventSystemConfig;
	g_theEventSystem = new EventSystem(eventSystemConfig);
	
	InputSystemConfig inputSystemConfig;
	g_theInput = new InputSystem(inputSystemConfig);
	
	WindowConfig windowConfig;
	windowConfig.m_windowTitle = "Stylized Rendering Tool";
	windowConfig.m_clientAspect = 2.0f;
	windowConfig.m_isFullScreen = true;
	windowConfig.m_inputSystem = g_theInput;
	g_theWindow = new Window(windowConfig);

	RendererConfig rendererConfig;
	rendererConfig.m_window = g_theWindow;
	g_theRenderer = new Renderer(rendererConfig);

	DevConsoleConfig devConsoleConfig;
	devConsoleConfig.m_camera = m_devCamera;
	devConsoleConfig.m_renderer = g_theRenderer;
	g_theDevConsole = new DevConsole(devConsoleConfig);

	AudioSystemConfig audioSystemConfig;
	g_theAudio = new AudioSystem(audioSystemConfig);

	DebugRenderConfig debugRenderConfig;
	debugRenderConfig.m_renderer = g_theRenderer;
	
	g_rng = new RandomNumberGenerator();

	//ImGui
	/*
	ImGuiBaseConfig imGuiSystemConfig;
	imGuiSystemConfig.m_renderer = g_theRenderer;
	imGuiSystemConfig.m_window = g_theWindow;*/

	//g_ImGui = new SRTEditor(imGuiSystemConfig);

	//g_theInput->Startup();
	g_theEventSystem->Startup();
	g_theWindow->Startup();
	g_theRenderer->Startup();
	g_theDevConsole->Startup();
	g_theInput->Startup();
	g_theAudio->Startup();
	DebugRenderSystemStartup(debugRenderConfig);
	//g_ImGui->Startup();


	ShaderManagerConfig shaderManagerConfig;
	shaderManagerConfig.m_defFilepath = "Data/Shaders/Engine/ShaderDefinitions";
	shaderManagerConfig.m_dimension = g_theRenderer->GetConfig().m_window->GetClientDimensions();
	shaderManagerConfig.m_renderer = g_theRenderer;

	g_shaderManager = new ShaderManager(shaderManagerConfig);

	m_devCamera->m_viewport = AABB2(Vec2(0.f, 0.f),
		Vec2((float)g_theRenderer->GetConfig().m_window->GetClientDimensions().x,
			(float)g_theRenderer->GetConfig().m_window->GetClientDimensions().y));

	g_theGameStatus = GameStatus::BASIC_3D;
	for (int i = 0; i < (int)GameStatus::NUM; i++) {
		Game::CreateOrGetNewGameOfType((GameStatus)i);
		if (g_theGames[i] != nullptr)
			g_theGames[i]->Startup();
	}
	g_theGame = Game::CreateOrGetNewGameOfType(g_theGameStatus);

	g_theGame->m_worldCamera.m_viewport = AABB2(Vec2(0.f, 0.f),
		Vec2((float)g_theRenderer->GetConfig().m_window->GetClientDimensions().x,
			(float)g_theRenderer->GetConfig().m_window->GetClientDimensions().y));

	g_theGame->m_screenCamera.m_viewport = AABB2(Vec2(0.f, 0.f),
		Vec2((float)g_theRenderer->GetConfig().m_window->GetClientDimensions().x,
			(float)g_theRenderer->GetConfig().m_window->GetClientDimensions().y));


	//SubscribeEventCallbackFunction("Quit", &Command_Quit);

	SubscribeEventCallbackObjectMethod("Quit", *g_theApp, &App::Event_Quit);

	g_theDevConsole->ExecuteXmlCommandScriptFile("Data/GameConfig.xml");

	TestForEventRecipient* test01 = new TestForEventRecipient();


	SubscribeEventCallbackObjectMethod("Test01", *test01, &TestForEventRecipient::TestEventRecipient01);
	SubscribeEventCallbackObjectMethod("Test02", *test01, &TestForEventRecipient::TestEventRecipient02);
	SubscribeEventCallbackObjectMethod("Test03", *test01, &TestForEventRecipient::TestEventRecipient03);

	delete test01;
	test01 = nullptr;

	ShowAllKeysOnDevConsole();
}

void App::Shutdown()
{
	UnsubscribeEventCallbackObjectMethod("Quit", *g_theApp, &App::Event_Quit);

	for (int i = 0; i < (int)GameStatus::NUM; i++) {
		if (g_theGames[i] != nullptr)
			g_theGames[i]->Shutdown();
	}

	for (int i = 0; i < (int)GameStatus::NUM; i++) {
		if (g_theGames[i] != nullptr) {
			delete g_theGames[i];
		}
		g_theGames[i] = nullptr;
	}
	delete g_shaderManager;
	g_shaderManager = nullptr;
	g_theEventSystem->Shutdown();
	g_theInput->Shutdown();
	g_theWindow->Shutdown();
	//g_ImGui->Shutdown();
	g_theRenderer->Shutdown();
	g_theAudio->Shutdown();
	g_theDevConsole->Shutdown();
	DebugRenderSystemShutdown();
	

	delete m_devCamera;

	delete g_theEventSystem;
	delete g_theDevConsole;
	delete g_theInput;
	delete g_theWindow;

	delete g_theRenderer;
	delete g_theAudio;
	delete g_ImGui;

	delete g_rng;

	g_theGame = nullptr;
	m_devCamera = nullptr;

	g_theEventSystem = nullptr;
	g_theDevConsole = nullptr;
	g_theInput = nullptr;
	g_theWindow = nullptr;
	g_theRenderer = nullptr;	
	g_theAudio = nullptr;
	g_ImGui = nullptr;

	g_rng = nullptr;

	
	
}

void App::RunFrame()
{
	Clock::TickSystemClock();
	BeginFrame();
	Update();
	Render();
	EndFrame();
}

void App::Run()
{
	while (!m_isQuitting)
	{
		RunFrame();
	}
}

bool App::HandleQuitRequested()
{
	m_isQuitting = true;
	return false;
}

bool App::Event_Quit(EventArgs& args)
{
	HandleQuitRequested();
	return false;
}

void App::BeginFrame()
{
	g_theEventSystem->BeginFrame();
	g_theDevConsole->BeginFrame();
	g_theInput->BeginFrame();
	g_theWindow->BeginFrame();
	g_theRenderer->BeginFrame();
	g_theAudio->BeginFrame();
	DebugRenderBeginFrame();
	g_shaderManager->BeginFrame();
	//g_ImGui->BeginFrame();
}

void App::Update()
{	
	UpdateFromKeyboard();
	m_devCamera->SetOrthoView(Vec2(0.f, 0.f), Vec2(SCREEN_CAMERA_SIZE_X, SCREEN_CAMERA_SIZE_Y));
	//g_ImGui->Update();
	g_theGames[(int)g_theGameStatus]->Update();
	bool mouse_show = true;
		//g_theDevConsole->IsOpen() || !g_theWindow->IsFocused();
	g_theInput->SetCursorMode(!mouse_show, !mouse_show);
}

void App::Render() const
{
	g_theRenderer->ClearScreen(Rgba8(0, 0, 0, 255));
	g_theGames[(int)g_theGameStatus]->Render();
	g_theDevConsole->Render(AABB2(0.f, 0.f, 1400.f, 700.f));
	//g_ImGui->Render();
}

void App::EndFrame()
{
	g_theEventSystem->EndFrame();
	g_theDevConsole->EndFrame();
	g_theInput->EndFrame();
	g_shaderManager->EndFrame();
	g_theWindow->EndFrame();
	g_theRenderer->EndFrame();
	g_theAudio->EndFrame();
	DebugRenderEndFrame();
	//g_ImGui->EndFrame();
}


void App::LoadAssets()
{
	TextureConfig filterTextureConfig;
	filterTextureConfig.m_name = "Fluid Blurred Texture";
	Vec2 dimension = Vec2((float)g_theRenderer->GetConfig().m_window->GetClientDimensions().x,
		(float)g_theRenderer->GetConfig().m_window->GetClientDimensions().y);
		filterTextureConfig.m_width = dimension.x;
	filterTextureConfig.m_height = dimension.y;
	filterTextureConfig.m_format = ResourceFormat::R8G8B8A8_UNORM;
	filterTextureConfig.m_bindFlags = RESOURCE_BINDFLAGS_SHADER_RESOURCE_VIEW | RESOURCE_BINDFLAGS_RENDER_TARGET_VIEW;
	filterTextureConfig.m_usage = MemoryUsage::DEFAULT;

	g_filterTexture = g_theRenderer->CreateTextureByConfig(filterTextureConfig);
	g_filterTexture->GetOrCreateResourceViewByFlag(RESOURCE_BINDFLAGS_RENDER_TARGET_VIEW);
	g_filterTexture->GetOrCreateResourceViewByFlag(RESOURCE_BINDFLAGS_SHADER_RESOURCE_VIEW);

	ShaderConfig kuwaharaConfig;
	kuwaharaConfig.m_name = "Data/Shaders/SepiaTone";
	//"Data/Shaders/Kuwahara";
	g_kuwaharaShader = g_theRenderer->CreateShaderByConfig(kuwaharaConfig);

	//m_renderCBO = g_theRenderer->CreateConstantBuffer(sizeof(RenderConstants));

	ShaderConfig copyConfig;
	copyConfig.m_name = "Data/Shaders/CopyToDefaultTargetView";
	g_copyShader = g_theRenderer->CreateShaderByConfig(copyConfig);
}

void App::UpdateFromKeyboard()
{
	if (g_theInput->WasKeyJustPressed('P'))
	{
		if (m_isPaused)
		{

		}
		else
		{

		}
		m_isPaused = !m_isPaused;
		
	}
	if (g_theInput->IsKeyDown('T'))
	{
		if (!m_isSlowMo) 
		{
			m_isSlowMo = true;
		}	
	}
	if (g_theInput->WasKeyJustPressed(KEYCODE_F8))
	{

	}
	if (g_theInput->WasKeyJustPressed(KEYCODE_F6)) {
		int now = (int)g_theGameStatus;
		if (now == 0)
			now = (int)GameStatus::NUM - 1;
		else now--;
		g_theGameStatus = GameStatus(now);
	}
	if (g_theInput->WasKeyJustPressed(KEYCODE_F7)) {
		int now = (int)g_theGameStatus;
		if (now == (int)GameStatus::NUM - 1)
			now = 0;
		else now++;
		g_theGameStatus = GameStatus(now);
	}
	if (g_theInput->WasKeyJustPressed(KEYCODE_ESC) || g_theInput->GetController(0).WasButtonJustPressed(XBOX_BUTTON_BACK))
	{
		HandleQuitRequested();
	}
	if (g_theInput->WasKeyJustReleased('T'))
	{
		if (m_isSlowMo)
		{
			m_isSlowMo = false;
		}
	}
	if (g_theInput->WasKeyJustPressed('B'))
	{

	}
}

void ShowAllKeysOnDevConsole()
{
	g_theDevConsole->AddLine(DevConsole::INFO_MINOR, "Type Help for a list of commands");
	g_theDevConsole->AddLine(DevConsole::INFO_MAJOR, "Keys");
	g_theDevConsole->AddLine(DevConsole::INFO_MINOR, "W				- Forward");
	g_theDevConsole->AddLine(DevConsole::INFO_MINOR, "S				- Back");
	g_theDevConsole->AddLine(DevConsole::INFO_MINOR, "A				- Left");
	g_theDevConsole->AddLine(DevConsole::INFO_MINOR, "D				- Right");
	g_theDevConsole->AddLine(DevConsole::INFO_MINOR, "Q				- Down");
	g_theDevConsole->AddLine(DevConsole::INFO_MINOR, "E				- Up");
	g_theDevConsole->AddLine(DevConsole::INFO_MINOR, "UP Arrow		- Pitch up");
	g_theDevConsole->AddLine(DevConsole::INFO_MINOR, "Down Arrow	- Pitch down");
	g_theDevConsole->AddLine(DevConsole::INFO_MINOR, "Left Arrow	- Yaw left");
	g_theDevConsole->AddLine(DevConsole::INFO_MINOR, "Right Arrow	- Yaw right");

}

TestForEventRecipient::TestForEventRecipient()
{

}

TestForEventRecipient::~TestForEventRecipient()
{

}
