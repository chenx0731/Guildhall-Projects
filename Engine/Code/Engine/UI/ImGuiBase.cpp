#include "Engine/UI/ImGuiBase.hpp"
#include "Engine/Renderer/Window.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "ThirdParty/ImGui/imgui.h"
#include "ThirdParty/ImGui/imgui_impl_dx11.h"
#include "ThirdParty/ImGui/imgui_impl_win32.h"
#include <d3d11.h>

ImGuiBase::ImGuiBase(ImGuiBaseConfig const& config) : m_config(config)
{
}

ImGuiBase::~ImGuiBase()
{
    
}

void ImGuiBase::Startup()
{
    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    //ImGui::StyleColorsDark();
    ImGui::StyleColorsLight();
    //ImGui::StyleColorsClassic();

    // Setup Platform/Renderer backends
    ImGui_ImplWin32_Init(m_config.m_window->GetHwnd());
    ImGui_ImplDX11_Init(m_config.m_renderer->GetD3D11Device(), m_config.m_renderer->GetD3D11DeviceContent());
}

void ImGuiBase::Shutdown()
{
    // Cleanup
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
	m_config.m_renderer = nullptr;
	m_config.m_window = nullptr;
}

void ImGuiBase::BeginFrame()
{
    // Start the Dear ImGui frame
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
}

void ImGuiBase::EndFrame()
{
}

void ImGuiBase::Update()
{
    if (m_isShowDemo)
        ImGui::ShowDemoWindow(&m_isShowDemo);
    ImGui::Begin("Hello, world!");
    ImGui::Text("This is some useful text.");
    ImGui::Checkbox("Demo Window", &m_isShowDemo);
    ImGui::End();
}

void ImGuiBase::Render() const
{
    ImGui::Render();
    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}
