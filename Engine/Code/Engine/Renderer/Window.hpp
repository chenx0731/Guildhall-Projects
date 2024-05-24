#pragma once
#include <string>
#include "Engine/Math/IntVec2.hpp"

class InputSystem;
struct Vec2;

struct WindowConfig
{
	InputSystem*	m_inputSystem = nullptr;
	std::string		m_windowTitle = "Untitled App";
	float			m_clientAspect = 2.0f;
	bool			m_isFullScreen = false;
	IntVec2			m_windowSize = IntVec2(-1, -1);
	IntVec2			m_windowPosition = IntVec2(-1, -1);
};

class Window
{
public:
	Window(WindowConfig const& config);
	~Window();

	void Startup();
	void BeginFrame();
	void EndFrame();
	void Shutdown();

	WindowConfig const& GetConfig() const;
	static Window* GetWindowContext();
	Vec2 GetNormalizedCursorPos() const;

	std::string FileOpenDialog();

	void* GetHwnd() const { return m_hwnd; };
	IntVec2 GetClientDimensions() const { return m_clientDimensions; };

	bool IsFocused();

protected:
	void CreateOSWindow();
	void RunMessagePump();

protected:
	WindowConfig	m_config;
	void*			m_hwnd;
	IntVec2			m_clientDimensions = IntVec2::Zero;
	static Window* s_mainWindow;
};