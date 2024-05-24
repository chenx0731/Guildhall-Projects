#define UNUSED(x) (void)(x);
#include "Game/Game.hpp"
#include "Game/GameCommon.hpp"
#include "Game/GameBasic3D.hpp"
#include "Game/GameBasic2D.hpp"
#include "Game/Entity.hpp"
#include "Game/Player.hpp"
#include "Game/Prop.hpp"
#include "Game/App.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Core/FileUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Core/Time.hpp"
#include "Engine/Renderer/SimpleTriangleFont.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Core/DebugRenderSystem.hpp"
#include "Engine/Renderer/Shader.hpp"

Game* g_theGames[(int)GameStatus::NUM];


Vertex_PCU vertices[] = {
	Vertex_PCU(Vec2(800.f, 600.f), Rgba8(255, 255, 255, 255), Vec2(0.f, 0.f)),
	Vertex_PCU(Vec2(400.f, 200.f), Rgba8(255, 255, 255, 255), Vec2(0.f, 0.f)),
	Vertex_PCU(Vec2(1200.f, 200.f), Rgba8(255, 255, 255, 255), Vec2(0.f, 0.f)),
};


Game::Game()
{
	m_clock = new Clock(Clock::GetSystemClock());	
	AABB2 viewport = AABB2(Vec2(0.f, 0.f),
		Vec2((float)g_theRenderer->GetConfig().m_window->GetClientDimensions().x,
			(float)g_theRenderer->GetConfig().m_window->GetClientDimensions().y));
	m_screenCamera.m_viewport = viewport;
	m_worldCamera.m_viewport = viewport;
}

Game::~Game()
{

}

void Game::Startup()
{
	LoadAssets();
	
	Vec2 worldCenter(WORLD_CENTER_X, WORLD_CENTER_Y);

	m_player = new Player(this, Vec3(0.f, 0.f, 0.f));
	m_player->m_camera.m_viewport = AABB2(Vec2(0.f, 0.f),
		Vec2((float)g_theRenderer->GetConfig().m_window->GetClientDimensions().x,
			(float)g_theRenderer->GetConfig().m_window->GetClientDimensions().y));
	m_prop1 = new Prop(this, Vec3(2.f, 2.f, 0.0f));
	//AddVertsForAABB3D(m_prop1->m_vertexes, AABB3(Vec3(-0.5f, -0.5f, -0.5f), Vec3(0.5f, 0.5f, 0.5f)));
	//m_prop1->m_texture = g_textureLibrary[TEXTURE_CUBE];
	float maxX = 0.5f;
	float maxY = 0.5f;
	float maxZ = 0.5f;
	float minX = -0.5f;
	float minY = -0.5f;
	float minZ = -0.5f;

	// +x
	AddVertsForQuad3D(m_prop1->m_vertexes, Vec3(maxX, minY, minZ), Vec3(maxX, maxY, minZ), Vec3(maxX, maxY, maxZ), Vec3(maxX, minY, maxZ), Rgba8(255, 0, 0));
	// -x					
	AddVertsForQuad3D(m_prop1->m_vertexes, Vec3(minX, maxY, minZ), Vec3(minX, minY, minZ), Vec3(minX, minY, maxZ), Vec3(minX, maxY, maxZ), Rgba8(0, 255, 255));
	// +y					
	AddVertsForQuad3D(m_prop1->m_vertexes, Vec3(maxX, maxY, minZ), Vec3(minX, maxY, minZ), Vec3(minX, maxY, maxZ), Vec3(maxX, maxY, maxZ), Rgba8(0, 255, 0));
	// -y					
	AddVertsForQuad3D(m_prop1->m_vertexes, Vec3(minX, minY, minZ), Vec3(maxX, minY, minZ), Vec3(maxX, minY, maxZ), Vec3(minX, minY, maxZ), Rgba8(255, 0, 255));
	// +z					
	AddVertsForQuad3D(m_prop1->m_vertexes, Vec3(minX, minY, maxZ), Vec3(maxX, minY, maxZ), Vec3(maxX, maxY, maxZ), Vec3(minX, maxY, maxZ), Rgba8(0, 0, 255));
	// -z					
	AddVertsForQuad3D(m_prop1->m_vertexes, Vec3(maxX, minY, minZ), Vec3(minX, minY, minZ), Vec3(minX, maxY, minZ), Vec3(maxX, maxY, minZ), Rgba8(255, 255, 0));

	m_prop2 = new Prop(this, Vec3(-2.f, -2.f, 0.f));
	
	// +x
	AddVertsForQuad3D(m_prop2->m_vertexes, Vec3(maxX, minY, minZ), Vec3(maxX, maxY, minZ), Vec3(maxX, maxY, maxZ), Vec3(maxX, minY, maxZ), Rgba8(255, 0, 0));
	// -x					
	AddVertsForQuad3D(m_prop2->m_vertexes, Vec3(minX, maxY, minZ), Vec3(minX, minY, minZ), Vec3(minX, minY, maxZ), Vec3(minX, maxY, maxZ), Rgba8(0, 255, 255));
	// +y					
	AddVertsForQuad3D(m_prop2->m_vertexes, Vec3(maxX, maxY, minZ), Vec3(minX, maxY, minZ), Vec3(minX, maxY, maxZ), Vec3(maxX, maxY, maxZ), Rgba8(0, 255, 0));
	// -y					
	AddVertsForQuad3D(m_prop2->m_vertexes, Vec3(minX, minY, minZ), Vec3(maxX, minY, minZ), Vec3(maxX, minY, maxZ), Vec3(minX, minY, maxZ), Rgba8(255, 0, 255));
	// +z					
	AddVertsForQuad3D(m_prop2->m_vertexes, Vec3(minX, minY, maxZ), Vec3(maxX, minY, maxZ), Vec3(maxX, maxY, maxZ), Vec3(minX, maxY, maxZ), Rgba8(0, 0, 255));
	// -z					
	AddVertsForQuad3D(m_prop2->m_vertexes, Vec3(maxX, minY, minZ), Vec3(minX, minY, minZ), Vec3(minX, maxY, minZ), Vec3(maxX, maxY, minZ), Rgba8(255, 255, 0));

	m_sphere = new Prop(this, Vec3(10.f, -5.f, 1.f));
	AddVertsForSphere3D(m_sphere->m_vertexes, Vec3(0.f, 0.f, 0.f), 1.f);
	//AddVertsForCylinder3D(m_sphere->m_vertexes, Vec3(1.f, 1.f, 1.f), Vec3(2.f, 2.f, 2.f), 0.5f);
	//AddVertsForCone3D(m_sphere->m_vertexes, Vec3(-1.f, -1.f, -1.f), Vec3(-2.f, -2.f, -2.f), 0.5f);
	//AddVertsForQuad3D(m_sphere->m_vertexes, Vec3(0.f, 1.f, 0.f), Vec3(0.f, 0.f, 0.f), Vec3(0.f, 0.f, 1.f), Vec3(0.f, 1.f, 1.f));
	m_sphere->m_texture = g_textureLibrary[TEXTURE_SPHERE];
	
	
	SetCamera();



}

Game* Game::CreateOrGetNewGameOfType(GameStatus type)
{
	if (g_theGames[(int)type] == nullptr) {
		switch (type)
		{
		case GameStatus::BASIC_2D:
			g_theGames[(int)type] = new GameBasic2D();
			break;
		case GameStatus::BASIC_3D:
			g_theGames[(int)type] = new GameBasic3D();
			break;
		default:
			return nullptr;
		}
	}

	return g_theGames[(int)type];
}


Shader* Game::GetShaderByEnum(PostProcessingType type) const
{
	switch (type)
	{
	case PostProcessingType::Kuwahara:
		return m_kuwaharaShader;
	case PostProcessingType::GlitchImageBlock:
		return m_GIBShader;
	case PostProcessingType::GlitchRGBSplit:
		return m_GRSShader;
	case PostProcessingType::GlitchAnologNoise:
		return m_GANShader;
	case PostProcessingType::PixelLeaf:
		return m_PLShader;
	case PostProcessingType::PixelHexagon:
		return m_PHGShader;
	default:
		return m_kuwaharaShader;
	}
}

void Game::SetRenderCBOByEnum(PostProcessingType type)
{
	switch (type)
	{
	case PostProcessingType::Kuwahara:
		SetKuwaharaCBO();
		break;
	case PostProcessingType::GlitchImageBlock:
		SetGIBCBO();
		break;
	case PostProcessingType::GlitchRGBSplit:
		SetGRSCBO();
		break;
	case PostProcessingType::GlitchAnologNoise:
		SetGANCBO();
		break;
	case PostProcessingType::PixelLeaf:
		SetPLCBO();
		break;
	case PostProcessingType::PixelHexagon:
		SetPHGCBO();
		break;
	default:
		SetKuwaharaCBO();
		break;
	}
}

void Game::LoadAssets()
{
	//g_musicLibrary[MUSIC_WELCOME] = g_theAudio->CreateOrGetSound("Data/Audio/Click.mp3");

	g_textureLibrary[TEXTURE_SPHERE] = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/TestUV.png");
	g_textureLibrary[TEXTURE_CUBE] = g_theRenderer->CreateOrGetTextureFromFile("Data/Images/Test_StbiFlippedAndOpenGL.png");

}


void Game::EnterGame()
{
	//SoundID testSound = g_theAudio->CreateOrGetSound("Data/Audio/Click.mp3");
	//PlayMusic(MUSIC_WELCOME);
	//g_theAudio->StartSound(testSound);
	Shutdown();
	Startup();
	m_isAttractMode = false;
	g_theInput->EndFrame();
}

void Game::SetCamera()
{
	//m_worldCamera.SetOrthoView(Vec2(-1.f, -1.f), Vec2(WORLD_CAMERA_SIZE_X, WORLD_CAMERA_SIZE_Y));
	m_player->m_camera.SetPerspectiveView(2.f, 60.f, 0.1f, 100.f);
	m_player->m_camera.SetRenderBasis(Vec3(0.f, 0.f, 1.f), Vec3(-1.f, 0.f, 0.f), Vec3(0.f, 1.f, 0.f));
	m_screenCamera.SetOrthoView(Vec2(0.f, 0.f), Vec2(SCREEN_CAMERA_SIZE_X, SCREEN_CAMERA_SIZE_Y));
}

void Game::SetKuwaharaCBO()
{
	RenderConstants renderConstants;
	renderConstants.InvViewSizeX = 1.f / m_worldCamera.m_viewport.GetDimensions().x;
	renderConstants.InvViewSizeY = 1.f / m_worldCamera.m_viewport.GetDimensions().y;
	//Vec2(1.f / m_worldCamera.m_viewport.GetDimensions().x, 1.f / m_worldCamera.m_viewport.GetDimensions().y);
	renderConstants.BlurRadiusX = 4.f;
	renderConstants.BlurRadiusY = 4.f;
	renderConstants.CursorXRatio = g_theWindow->GetNormalizedCursorPos().x;

	//renderConstants.TotalTime = m_clock->GetTotalSeconds();
	//renderConstants.Seed = 27.f;
	//renderConstants.Offset = 10.f;
	renderConstants.TotalTime = m_clock->GetTotalSeconds();
	renderConstants.Fade = 1.f;
	renderConstants.Speed = 10.f;

	renderConstants.BlockLayer2U = 2.f;
	renderConstants.BlockLayer2V = 100.f;
	renderConstants.BlockLayer1Intensity = 10.f;
	renderConstants.BlockLayer2Intensity = 10.f;
	renderConstants.BlockSplitIntensity = 10.f;

	g_theRenderer->CopyCPUToGPU(&renderConstants, sizeof(renderConstants), m_renderCBO);
	g_theRenderer->BindConstantBuffer(k_renderConstantsSlot, m_renderCBO);
}

void Game::SetGIBCBO()
{
	RenderConstants renderConstants;
	renderConstants.CursorXRatio = g_theWindow->GetNormalizedCursorPos().x;
	renderConstants.TotalTime = m_clock->GetTotalSeconds();
	renderConstants.Fade = 1.f;
	renderConstants.Speed = 30.f;

	renderConstants.Seed = 27.f;
	renderConstants.Offset = 10.f;

	renderConstants.BlockLayer1U = 20.f;
	renderConstants.BlockLayer1V = 50.f;
	renderConstants.BlockLayer2U = 30.f;
	renderConstants.BlockLayer2V = 70.f;
	renderConstants.BlockLayer1Intensity = 10.f;
	renderConstants.BlockLayer2Intensity = 10.f;
	renderConstants.BlockSplitIntensity = 10.f;

	g_theRenderer->CopyCPUToGPU(&renderConstants, sizeof(renderConstants), m_renderCBO);
	g_theRenderer->BindConstantBuffer(k_renderConstantsSlot, m_renderCBO);
}

void Game::SetGRSCBO()
{
	RenderConstants renderConstants;
	renderConstants.CursorXRatio = g_theWindow->GetNormalizedCursorPos().x;
	renderConstants.TotalTime = m_clock->GetTotalSeconds();
	renderConstants.Fade = 1.f;
	renderConstants.Speed = 10.f;

	renderConstants.Amount = 1.f;
	renderConstants.CenterFading = 1.f;
	renderConstants.AmountR = 5.f;
	renderConstants.AmountB = 7.f;


	g_theRenderer->CopyCPUToGPU(&renderConstants, sizeof(renderConstants), m_renderCBO);
	g_theRenderer->BindConstantBuffer(k_renderConstantsSlot, m_renderCBO);
}

void Game::SetGANCBO()
{
	RenderConstants renderConstants;
	renderConstants.CursorXRatio = g_theWindow->GetNormalizedCursorPos().x;
	renderConstants.TotalTime = m_clock->GetTotalSeconds();
	renderConstants.Fade = 1.f;
	renderConstants.Speed = 10.f;

	renderConstants.LuminanceJitterThreshold = 0.595f;

	g_theRenderer->CopyCPUToGPU(&renderConstants, sizeof(renderConstants), m_renderCBO);
	g_theRenderer->BindConstantBuffer(k_renderConstantsSlot, m_renderCBO);
}

void Game::SetPLCBO()
{
	RenderConstants renderConstants;
	renderConstants.CursorXRatio = g_theWindow->GetNormalizedCursorPos().x;
	renderConstants.TotalTime = m_clock->GetTotalSeconds();
	renderConstants.Fade = 1.f;
	renderConstants.Speed = 10.f;

	renderConstants.PixelScaleX = 0.8f;
	renderConstants.PixelScaleY = 0.8f;
	renderConstants.PixelSize = 100.f;
	renderConstants.PixelRatio = 2.f;
	g_theRenderer->CopyCPUToGPU(&renderConstants, sizeof(renderConstants), m_renderCBO);
	g_theRenderer->BindConstantBuffer(k_renderConstantsSlot, m_renderCBO);
}

void Game::SetPHGCBO()
{
	RenderConstants renderConstants;
	renderConstants.InvViewSizeX = 1.f / m_worldCamera.m_viewport.GetDimensions().x;
	renderConstants.InvViewSizeY = 1.f / m_worldCamera.m_viewport.GetDimensions().y;
	//Vec2(1.f / m_worldCamera.m_viewport.GetDimensions().x, 1.f / m_worldCamera.m_viewport.GetDimensions().y);
	renderConstants.PixelSize = 0.03f;
	renderConstants.GridWidth = 1.5f;
	renderConstants.CursorXRatio = g_theWindow->GetNormalizedCursorPos().x;

	//renderConstants.TotalTime = m_clock->GetTotalSeconds();
	//renderConstants.Seed = 27.f;
	//renderConstants.Offset = 10.f;
	renderConstants.TotalTime = m_clock->GetTotalSeconds();
	renderConstants.Fade = 1.f;

	renderConstants.BlockLayer1U = 0.03f;
	renderConstants.BlockLayer1V = 1.5f;
	renderConstants.BlockLayer2U = 2.f;
	renderConstants.BlockLayer2V = 100.f;
	renderConstants.BlockLayer1Intensity = 10.f;
	renderConstants.BlockLayer2Intensity = 10.f;
	renderConstants.BlockSplitIntensity = 10.f;

	g_theRenderer->CopyCPUToGPU(&renderConstants, sizeof(renderConstants), m_renderCBO);
	g_theRenderer->BindConstantBuffer(k_renderConstantsSlot, m_renderCBO);
}

