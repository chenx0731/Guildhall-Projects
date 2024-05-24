#include "Engine/Renderer/SRTEditor.hpp"
#include "Engine/Renderer/Window.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "ThirdParty/ImGui/imgui.h"
#include "ThirdParty/ImGui/imgui_impl_dx11.h"
#include "ThirdParty/ImGui/imgui_impl_win32.h"
#include <d3d11.h>
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/ShaderDefinition.hpp"
#include "Engine/Renderer/ShaderManager.hpp"

SRTEditor::SRTEditor(ImGuiBaseConfig const& config) : ImGuiBase(config)
{
    m_currentEffectName = "Kuwahara";
}

SRTEditor::~SRTEditor()
{
}

void SRTEditor::Update()
{
    //if (m_isShowDemo)
        //ImGui::ShowDemoWindow(&m_isShowDemo);
    ImGui::Begin("Effects Editor Engine");
    //ImGui::Text("This is some useful text.");
    //ImGui::Checkbox("Demo Window", &m_isShowDemo);
    //const int effectsNum = (int)m_shaderManager->m_effects.size();
    /*
    char** screenViews = nullptr;
    screenViews = new char* [m_shaderManager->GetAllEffectCount()];
    screenViews = (char**)m_shaderManager->m_effectNames.data();*/
        //new char [m_shaderManager->GetAllEffectCount()];
        //{ "Depth", "Blurred Depth", "Thickness", "Blurred Thickness", "Normal Map", "Final Merged", "Background" };
        //m_shaderManager->m_effectNames.data();
        //->GetAllEffectNames();

    //
    const char* screenViews[] = {"Kuwahara", "PixelizeLeaf", "PixelizeHexagonGrid", "GlitchAnalogNoise", "GlitchImageBlock", "GlitchRGBSplit"};
	//static const char* item_current = screenViews[0];            // Here our selection is a single pointer stored outside the object.
    if (ImGui::Combo("Effect", &(m_currentEffectIndex), screenViews, IM_ARRAYSIZE(screenViews)))
    {
        m_currentEffectName = screenViews[m_currentEffectIndex];
    }
    
    ShaderDefinition* shader = m_shaderManager->GetEffectByName(m_currentEffectName.c_str());
    if (shader)
    {
        for (int i = 0; i < (int)shader->m_channels.size(); i++)
        {
            if (shader->m_channels[i].m_visibility)
            {
                ImGui::DragFloat(shader->m_channels[i].m_name.c_str(),
                    &(shader->m_channels[i].m_default),
                    shader->m_channels[i].m_speed,
                    shader->m_channels[i].m_min,
                    shader->m_channels[i].m_max,
                    "%.2f");
            }
        }
    }
    if (ImGui::Button("Save"))
    {
        m_shaderManager->WriteIntoFile();
    }
    ImGui::End();
}
