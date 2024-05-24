#pragma once
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Core/HashedCaseInsensitiveString.hpp"
#include "Engine/Math/Vec2.hpp"

#include <string>
#include <map>

class ShaderDefinition;
class ConstantBuffer;
class Renderer;
class Shader;
class Texture;
class Clock;
class SRTEditor;

enum class ShaderManagerEditorMode
{
	Disabled,
	Normal,
	CompareOne,
};

struct ShaderManagerConfig
{
	std::string m_defFilepath = "";
	Renderer*	m_renderer = nullptr;
	IntVec2     m_dimension;
	//Clock*		m_clock = nullptr;
};

struct ShaderManagerConstants
{
	Vec2  CursorRatio;
	Vec2  InvViewSize;

	float TotalTime;
	int	  ComparisonMode;
	float Padding0;
	float Padding1;
};
static const int k_shaderManagerConstantsSlot = 5;

struct RenderingConstants
{
	float Channels[16];
	/*
	union Channel0
	{
		float	Float0;
		int		Int0;
		bool	Bool0;
	};*/
};
static const int k_renderingConstantsSlot = 4;
static const int k_bindingTextureSlot = 2;


class ShaderManager
{
public:
	ShaderManager(ShaderManagerConfig const& config);
	~ShaderManager();

	void Startup();
	void Shutdown();
	void BeginFrame();
	void EndFrame();

	void ApplyEffectByName(const char* name);
	void SetRenderingCBOByName(const char* name);
	void SetRenderingCBOByEffect(ShaderDefinition* shaderDef);
	const char** GetAllEffectNames();
	int GetAllEffectCount() const;

	void WriteIntoFile();
	ShaderDefinition* GetEffectByName(const char* name);
	std::vector<const char*>				m_effectNames;
private:
	ShaderManagerConfig						m_config;
	//std::vector<ShaderDefinition*>	m_effects;
	std::map<HCIString, ShaderDefinition*>	m_effects;
	
	ConstantBuffer*							m_shaderManagerCBO = nullptr;
	ConstantBuffer*							m_renderingCBO = nullptr;
	Shader*									m_copyShader = nullptr;
	Texture*								m_tempBuffer = nullptr;
	ShaderManagerEditorMode					m_editorMode = ShaderManagerEditorMode::Normal;
	SRTEditor*								m_editor = nullptr;
	Vec2									m_invViewSize;
};
