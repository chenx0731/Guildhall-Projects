#include "Engine/Renderer/ShaderManager.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/ShaderDefinition.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Renderer/ConstantBuffer.hpp"
#include "Engine/Renderer/Texture.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Core/FileUtils.hpp"
#include "Engine/Renderer/Window.hpp"
#include "Engine/Renderer/SRTEditor.hpp"

ShaderManager::ShaderManager(ShaderManagerConfig const& config) : m_config(config)
{
	std::string defPathDefault = m_config.m_defFilepath + ".default.xml";
	std::string defPathUser = m_config.m_defFilepath + ".user.xml";
	std::string defPath;
	if (IsFileExist(defPathUser))
		defPath = defPathUser;
	else defPath = defPathDefault;
	XmlDocument document;
	document.LoadFile(defPath.c_str());
	XmlElement* element = document.FirstChildElement();
	element = element->FirstChildElement();
	while (element)
	{
		ShaderDefinition* shaderDef = new ShaderDefinition(*element);
		m_effects[shaderDef->m_name] = shaderDef;
		m_effectNames.push_back(shaderDef->m_name.c_str());
		element = element->NextSiblingElement();
	}
	m_renderingCBO = m_config.m_renderer->CreateConstantBuffer(sizeof(RenderingConstants));
	m_shaderManagerCBO = m_config.m_renderer->CreateConstantBuffer(sizeof(ShaderManagerConstants));
	m_copyShader = m_config.m_renderer->CreateShader("Data/Shaders/Engine/CopyToDefaultTargetView");
	TextureConfig tempBufferConfig;
	tempBufferConfig.m_name = "Fluid Blurred Texture";
	tempBufferConfig.m_width = m_config.m_dimension.x;
	tempBufferConfig.m_height = m_config.m_dimension.y;
	tempBufferConfig.m_format = ResourceFormat::R8G8B8A8_UNORM;
	tempBufferConfig.m_bindFlags = RESOURCE_BINDFLAGS_SHADER_RESOURCE_VIEW | RESOURCE_BINDFLAGS_RENDER_TARGET_VIEW;
	tempBufferConfig.m_usage = MemoryUsage::DEFAULT;

	m_tempBuffer = m_config.m_renderer->CreateTextureByConfig(tempBufferConfig);
	m_tempBuffer->GetOrCreateResourceViewByFlag(RESOURCE_BINDFLAGS_RENDER_TARGET_VIEW);
	m_tempBuffer->GetOrCreateResourceViewByFlag(RESOURCE_BINDFLAGS_SHADER_RESOURCE_VIEW);

	ImGuiBaseConfig imguiConfig;
	imguiConfig.m_renderer = config.m_renderer;
	imguiConfig.m_window = config.m_renderer->GetConfig().m_window;
	m_editor = new SRTEditor(imguiConfig);
	m_editor->m_shaderManager = this;
	m_editor->Startup();
	m_invViewSize = Vec2(1.f / (float)config.m_dimension.x, 1.f / (float)config.m_dimension.y);
}

ShaderManager::~ShaderManager()
{
	delete m_renderingCBO;
	m_renderingCBO = nullptr;

	delete m_shaderManagerCBO;
	m_shaderManagerCBO = nullptr;

	m_editor->Shutdown();
	delete m_editor;
	m_editor = nullptr;

	for (auto it = m_effects.begin(); it != m_effects.end(); it++)
	{
		if (it->second)
		{
			delete it->second;
			it->second = nullptr;
		}
	}

	m_effects.clear();
	m_copyShader = nullptr;
	m_config.m_renderer = nullptr;
	m_tempBuffer = nullptr;
}

void ShaderManager::Startup()
{

}

void ShaderManager::Shutdown()
{
	
}

void ShaderManager::BeginFrame()
{
	//m_editor->BeginFrame();
	if (m_editorMode != ShaderManagerEditorMode::Disabled)
	{
		m_editor->BeginFrame();
		m_editor->Update();
	}
}

void ShaderManager::EndFrame()
{
	if (m_editorMode != ShaderManagerEditorMode::Disabled)
	{
		ApplyEffectByName(m_editor->m_currentEffectName.c_str());
		m_editor->Render();
		m_editor->EndFrame();
	}
}

void ShaderManager::ApplyEffectByName(const char* name)
{
	ShaderDefinition* shader = GetEffectByName(name);
	SetRenderingCBOByEffect(shader);
	
	m_config.m_renderer->ClearRenderTarget(Rgba8(0, 0, 0, 0), m_tempBuffer);
	m_config.m_renderer->SetDepthTexture(nullptr, m_tempBuffer);
	m_config.m_renderer->BindBackBufferAsTexture(true, k_bindingTextureSlot);
	m_config.m_renderer->BindShader(shader->m_shader);
	m_config.m_renderer->Draw(3);
	//copy to default render target
	m_config.m_renderer->BindBackBufferAsTexture(false, 2);
	m_config.m_renderer->ClearRenderTarget(Rgba8(0, 0, 0, 0));
	m_config.m_renderer->SetDepthTexture(nullptr);
	m_config.m_renderer->BindTexture(m_tempBuffer, k_bindingTextureSlot);
	m_config.m_renderer->BindShader(m_copyShader);
	m_config.m_renderer->Draw(3);
	m_config.m_renderer->BindShader(nullptr);
	m_config.m_renderer->BindTexture(nullptr, k_bindingTextureSlot);

}

void ShaderManager::SetRenderingCBOByName(const char* name)
{
	ShaderDefinition* shader = GetEffectByName(name);
	SetRenderingCBOByEffect(shader);
}

void ShaderManager::SetRenderingCBOByEffect(ShaderDefinition* shaderDef)
{
	ShaderManagerConstants smConstant;
	smConstant.CursorRatio = m_config.m_renderer->GetConfig().m_window->GetNormalizedCursorPos();
	smConstant.ComparisonMode = (int)m_editorMode;
	smConstant.InvViewSize = m_invViewSize;
	smConstant.TotalTime = Clock::GetSystemClock().GetTotalSeconds();
	m_config.m_renderer->CopyCPUToGPU(&smConstant, sizeof(ShaderManagerConstants), m_shaderManagerCBO);
	m_config.m_renderer->BindConstantBuffer(k_shaderManagerConstantsSlot, m_shaderManagerCBO);
	
	RenderingConstants rConstant;
	if (shaderDef)
	{
		for (int i = 0; i < (int)shaderDef->m_channels.size(); i++)
		{
			rConstant.Channels[i] = shaderDef->m_channels[i].m_default;

		}

		m_config.m_renderer->CopyCPUToGPU(&rConstant, sizeof(RenderingConstants), m_renderingCBO);
		m_config.m_renderer->BindConstantBuffer(k_renderingConstantsSlot, m_renderingCBO);
	}

}

const char** ShaderManager::GetAllEffectNames()
{
	return m_effectNames.data();
}

int ShaderManager::GetAllEffectCount() const
{
	return (int)m_effectNames.size();
}

void ShaderManager::WriteIntoFile()
{
	XmlDocument xmlDoc;
	XmlElement* element = xmlDoc.NewElement("ShaderDefinitions");
	xmlDoc.InsertFirstChild(element);
	for (auto it = m_effects.begin(); it != m_effects.end(); it++)
	{
		if (it->second)
		{
			it->second->WriteIntoXmlDoc(xmlDoc, *element);
		}
	}
	xmlDoc.InsertEndChild(element);
	xmlDoc.SaveFile("Data/Shaders/Engine/ShaderDefinitions.user.xml");
}

ShaderDefinition* ShaderManager::GetEffectByName(const char* name)
{
	ShaderDefinition* res = nullptr;
	if (m_effects.find(name) != m_effects.end())
		res = m_effects[name];
	return res;
}

