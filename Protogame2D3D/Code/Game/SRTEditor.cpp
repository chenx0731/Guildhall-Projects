#include "Game/SRTEditor.hpp"
#include "Engine/Renderer/Window.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "ThirdParty/ImGui/imgui.h"
#include "ThirdParty/ImGui/imgui_impl_dx11.h"
#include "ThirdParty/ImGui/imgui_impl_win32.h"
#include <d3d11.h>

SRTEditor::SRTEditor(ImGuiBaseConfig const& config) : ImGuiBase(config)
{
}

SRTEditor::~SRTEditor()
{
}

void SRTEditor::Update()
{
    if (m_isShowDemo)
        ImGui::ShowDemoWindow(&m_isShowDemo);
    ImGui::Begin("Hello, world!");
    ImGui::Text("This is some useful text.");
    ImGui::Checkbox("Demo Window", &m_isShowDemo);
    ImGui::End();
}
