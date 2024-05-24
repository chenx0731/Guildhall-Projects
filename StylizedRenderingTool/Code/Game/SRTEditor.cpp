#include "Game/SRTEditor.hpp"
#include "Game/GameCommon.hpp"
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
    ImGui::Begin("Effects Editor Game");
    //ImGui::Text("This is some useful text.");
    //ImGui::Checkbox("Demo Window", &m_isShowDemo);
    ShaderDefinition* shader = g_shaderManager->GetEffectByName(m_currentEffectName.c_str());
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
        g_shaderManager->WriteIntoFile();
    }
    ImGui::End();
}
