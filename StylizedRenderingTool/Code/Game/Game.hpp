#pragma once
#include "Engine/Renderer/Camera.hpp"
#include "Engine/Core/Vertex_PCU.hpp"

class App;
class Entity;
class Player;
class Prop;
class Clock;
class Texture;
class Shader;
class ConstantBuffer;

struct RenderConstants
{
	float CursorXRatio;
	float TotalTime;
	float Fade;
	float Speed;

	union //Channel4
	{
		float InvViewSizeX; // kuwahara & pixel hexagon
		float Seed; // glitch Image
		float Amount; // glitch rgb
		float LuminanceJitterThreshold; // glitch analog noise
		float PixelScaleX; // pixel leaf
		//float PixelSize; // pixel hexagon
	};

	union //Channel5
	{
		float InvViewSizeY;
		float Offset;
		float CenterFading;
		float PixelScaleY;
	};

	union //Channel6
	{
		float BlurRadiusX;
		float BlockLayer1U;
		float AmountR;
		float PixelSize;
	};

	union //Channel7
	{
		float BlurRadiusY;
		float BlockLayer1V;
		float AmountB;
		float PixelRatio;
		float GridWidth;
	};

	union //Channel8
	{
		float BlockLayer2U;
	};
	union //Channel9
	{
		float BlockLayer2V;
	};
	union //Channel10
	{
		float BlockLayer1Intensity;
	};
	union //Channel11
	{
		float BlockLayer2Intensity;
	};
	union //Channel12
	{
		float BlockSplitIntensity;
	};

	float Padding0;
	float Padding1;
	float Padding2;
};
static const int k_renderConstantsSlot = 4;

enum class PostProcessingType
{
	Kuwahara,
	GlitchImageBlock,
	GlitchRGBSplit,
	GlitchAnologNoise,
	PixelLeaf,
	PixelHexagon,

	Shader_Num,
};

struct Vec2;

enum class GameStatus
{
	NONE = -1,

	BASIC_2D,
	BASIC_3D,

	NUM,
};

class Game
{
public:
	Game();
	virtual ~Game();
	virtual void Startup();
	virtual void Update() = 0;
	virtual void Render() const = 0;
	virtual void Shutdown() {};

	virtual void Enter() = 0;
	virtual void Exit() = 0;
	
	static Game* CreateOrGetNewGameOfType(GameStatus type);

	Shader* GetShaderByEnum(PostProcessingType type) const;

	void SetRenderCBOByEnum(PostProcessingType type);

private:

	void LoadAssets();

	void EnterGame();
	void SetCamera();
	void SetKuwaharaCBO();
	void SetGIBCBO();
	void SetGRSCBO();
	void SetGANCBO();
	void SetPLCBO();
	void SetPHGCBO();
public:
	GameStatus	m_status;

	Camera		m_worldCamera;
	Camera		m_screenCamera;

	bool		m_isDebug = false;
	bool		m_isPaused = false;
	bool		m_isShaking = false;
	bool		m_isAttractMode = true;
	bool		m_isOver = false;

	float		m_hangingTime = 0.f;
	int			m_current_level = 0;
	float		m_shakingTime = 0.f;

	Player*		m_player;
	Prop*		m_prop1;
	Prop*		m_prop2;
	Prop*		m_sphere;
	Shader*		m_shader = nullptr;

	Texture* m_filterTexture = nullptr;

	Shader* m_kuwaharaShader = nullptr;
	Shader* m_GIBShader = nullptr;
	Shader* m_GRSShader = nullptr;
	Shader* m_GANShader = nullptr;
	Shader* m_PLShader = nullptr;
	Shader* m_PHGShader = nullptr;

	Shader* m_copyShader = nullptr;

	Clock*		m_clock;

	ConstantBuffer* m_renderCBO = nullptr;
	PostProcessingType m_currentType = PostProcessingType::Kuwahara;
};