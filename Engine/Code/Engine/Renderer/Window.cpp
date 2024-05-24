#pragma once
#define UNUSED(x) (void)(x);

#include "Engine/Renderer/Window.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"
#include "Engine/Math/Vec2.hpp"
#include "ThirdParty/ImGui/imgui_impl_win32.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <commdlg.h>
#include <math.h>
#include <cassert>
#include <crtdbg.h>

Window* Window::s_mainWindow = nullptr;
HDC				m_displayContext = nullptr;

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK WindowsMessageHandlingProcedure(HWND windowHandle, UINT wmMessageCode, WPARAM wParam, LPARAM lParam)
{
	Window* windowContext = Window::GetWindowContext();
	GUARANTEE_OR_DIE(windowContext != nullptr, "WindowContext was null!");
	InputSystem* input = windowContext->GetConfig().m_inputSystem;
	GUARANTEE_OR_DIE(input != nullptr, "No Input System!");

	if (ImGui_ImplWin32_WndProcHandler(windowHandle, wmMessageCode, wParam, lParam))
		return true;

	switch (wmMessageCode)
	{
		// App close requested via "X" button, or right-click "Close Window" on task bar, or "Close" from system menu, or Alt-F4
	case WM_CLOSE:
	{
		FireEvent("Quit");
		return 0;
	}

	case WM_CHAR: 
	{
		EventArgs args;
		args.SetValue("CharCode", Stringf("%d", (unsigned char)wParam));
		FireEvent("CharInput", args);
		return 0;
	}

	// Raw physical keyboard "key-was-just-depressed" event (case-insensitive, not translated)
	case WM_KEYDOWN:
	{
		EventArgs args;
		args.SetValue("KeyCode", Stringf("%d", (unsigned char)wParam));
		FireEvent("KeyPressed", args);
		return 0;
	}

	// Raw physical keyboard "key-was-just-released" event (case-insensitive, not translated)
	case WM_KEYUP:
	{
		EventArgs args;
		args.SetValue("KeyCode", Stringf("%d", (unsigned char)wParam));
		FireEvent("KeyReleased", args);
		return 0;
	}

	case WM_LBUTTONDOWN:
	{
		unsigned char keyCode = KEYCODE_LEFT_MOUSE;
		if (input) {
			input->HandleKeyPressed(keyCode);
			return 0;
		}
	}

	case WM_LBUTTONUP:
	{
		unsigned char keyCode = KEYCODE_LEFT_MOUSE;
		if (input) {
			input->HandleKeyReleased(keyCode);
			return 0;
		}
	}

	case WM_RBUTTONDOWN:
	{
		unsigned char keyCode = KEYCODE_RIGHT_MOUSE;
		if (input) {
			input->HandleKeyPressed(keyCode);
			return 0;
		}
	}

	case WM_RBUTTONUP:
	{
		unsigned char keyCode = KEYCODE_RIGHT_MOUSE;
		if (input) {
			input->HandleKeyReleased(keyCode);
			return 0;
		}
	}

	}

	// Send back to Windows any unhandled/unconsumed messages we want other apps to see (e.g. play/pause in music apps, etc.)
	return DefWindowProc(windowHandle, wmMessageCode, wParam, lParam);
}

Window::Window(WindowConfig const& config) : m_config(config)
{
	s_mainWindow = this;
}

Window::~Window()
{
}

void Window::Startup()
{
	CreateOSWindow();
}

void Window::BeginFrame()
{
	RunMessagePump();
}

void Window::EndFrame()
{
	//SwapBuffers(m_displayContext);
}

void Window::Shutdown()
{
}

WindowConfig const& Window::GetConfig() const
{
	return m_config;
}

Window* Window::GetWindowContext()
{
	return s_mainWindow;
}

Vec2 Window::GetNormalizedCursorPos() const
{
	//HWND windowHandle = ::GetActiveWindow();
	//	//= HWND(m_hwnd);
	//POINT cursorCoords;
	//RECT clientRect;
	//::GetCursorPos(&cursorCoords);
	//::ScreenToClient(windowHandle, &cursorCoords);
	//::GetClientRect(windowHandle, &clientRect);
	//float cursorX = static_cast<float> (cursorCoords.x) / static_cast<float> (clientRect.right - clientRect.left);
	//float cursorY = static_cast<float> (cursorCoords.y) / static_cast<float> (clientRect.bottom - clientRect.top);
	//return Vec2(cursorX, 1.f - cursorY);

	POINT cursorCoords;
	::GetCursorPos(&cursorCoords);

	HWND windowHandle = (HWND)(Window::GetWindowContext()->GetHwnd());
	::ScreenToClient(windowHandle, &cursorCoords);
	RECT clientRect;
	::GetClientRect(windowHandle, &clientRect);
	float mouseClientX = float(cursorCoords.x);
	float mouseClientY = float(cursorCoords.y);
	float clientWidth = float(clientRect.right - clientRect.left);
	float clientHight = float(clientRect.bottom);
	return Vec2(mouseClientX / clientWidth, 1.f - (mouseClientY / clientHight));
}

std::string Window::FileOpenDialog()
{
	// get current directory
	char currentDir[MAX_PATH] = { '\0'};
	GetCurrentDirectoryA(MAX_PATH, currentDir);
	// open file

	OPENFILENAMEA openFile = { 0 };
	char Buffer[MAX_PATH] = { '\0'};
	//fill(Buffer, Buffer + 300, '\0');
	openFile.lStructSize = sizeof(OPENFILENAMEA);
	openFile.hwndOwner = NULL;
		//(HWND)m_hwnd;
	openFile.lpstrFile = Buffer;
	openFile.nMaxFile = MAX_PATH;
	openFile.Flags = OFN_EXPLORER | OFN_NOCHANGEDIR;
	openFile.lpstrFilter = ("Format: XML\0*.xml\0");
		//NULL;
	openFile.lpstrCustomFilter = NULL;
	openFile.lpstrFileTitle = NULL;
	openFile.lpstrInitialDir = currentDir;
	openFile.lpstrTitle = NULL;
	//LPOPENFILENAMEA openFileLP = &openFile;
	bool result;
	result = GetOpenFileNameA(&openFile);
	if (!result)
		return std::string();
	// set current directory to be the original path
	SetCurrentDirectoryA(currentDir);
	DWORD errorID = GetLastError();
	if (errorID != 0) {
		LPSTR messageBuffer = nullptr;
		size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
			NULL, errorID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);
		std::string message(messageBuffer, size);
		LocalFree(messageBuffer);
		DebuggerPrintf(message.c_str());
	}
	std::string fileName = openFile.lpstrFile;
	//LocalFree(openFile);
	return fileName;
}

bool Window::IsFocused()
{
	HWND windowHandle = ::GetActiveWindow();
	if (windowHandle == s_mainWindow->GetHwnd())
		return true;
	return false;
}

void Window::CreateOSWindow()
{
	// Define a window style/class
	WNDCLASSEX windowClassDescription;
	memset(&windowClassDescription, 0, sizeof(windowClassDescription));
	windowClassDescription.cbSize = sizeof(windowClassDescription);
	windowClassDescription.style = CS_OWNDC; // Redraw on move, request own Display Context
	windowClassDescription.lpfnWndProc = static_cast<WNDPROC>(WindowsMessageHandlingProcedure); // Register our Windows message-handling function
	windowClassDescription.hInstance = GetModuleHandle(NULL);
	windowClassDescription.hIcon = NULL;
	windowClassDescription.hCursor = NULL;
	windowClassDescription.lpszClassName = TEXT("Simple Window Class");
	RegisterClassEx(&windowClassDescription);

	// #SD1ToDo: Add support for fullscreen mode (requires different window style flags than windowed mode)
	DWORD windowStyleFlags = WS_CAPTION | WS_BORDER | WS_THICKFRAME | WS_SYSMENU | WS_OVERLAPPED;
	if (m_config.m_isFullScreen) {
		windowStyleFlags = WS_POPUP;
	}
	const DWORD windowStyleExFlags = WS_EX_APPWINDOW;

	// Get desktop rect, dimensions, aspect
	RECT desktopRect;
	HWND desktopWindowHandle = GetDesktopWindow();
	GetClientRect(desktopWindowHandle, &desktopRect);
	float desktopWidth = (float)(desktopRect.right - desktopRect.left);
	float desktopHeight = (float)(desktopRect.bottom - desktopRect.top);
	float desktopAspect = desktopWidth / desktopHeight;

	// Calculate maximum client size (as some % of desktop size)
	constexpr float maxClientFractionOfDesktop = 1.0f;
	float clientWidth = desktopWidth * maxClientFractionOfDesktop;
	float clientHeight = desktopHeight * maxClientFractionOfDesktop;
	if (m_config.m_clientAspect > desktopAspect)
	{
		// Client window has a wider aspect than desktop; shrink client height to match its width
		clientHeight = clientWidth / m_config.m_clientAspect;
	}
	else
	{
		// Client window has a taller aspect than desktop; shrink client width to match its height
		clientWidth = clientHeight * m_config.m_clientAspect;
	}
	if (m_config.m_isFullScreen) {
		clientWidth = desktopWidth;
		clientHeight = desktopHeight;
	}
	else {
		if (m_config.m_windowSize != IntVec2(-1, -1)) {
			clientWidth = static_cast<float>(m_config.m_windowSize.x);
			clientHeight = static_cast<float>(m_config.m_windowSize.y);
		}
	}
	m_clientDimensions.y = static_cast<int>(clientHeight);
	m_clientDimensions.x = static_cast<int>(clientWidth);
	// Calculate client rect bounds by centering the client area
	float clientMarginX = 0.5f * (desktopWidth - clientWidth);
	float clientMarginY = 0.5f * (desktopHeight - clientHeight);
	RECT clientRect;
	clientRect.left = (int)clientMarginX;
	clientRect.right = clientRect.left + (int)clientWidth;
	clientRect.top = (int)clientMarginY;
	clientRect.bottom = clientRect.top + (int)clientHeight;
	if (m_config.m_windowPosition != IntVec2(-1, -1)) {
		clientRect.left = m_config.m_windowPosition.x;
		clientRect.top = m_config.m_windowPosition.y;
		clientRect.right = clientRect.left + (int)clientWidth;
		clientRect.bottom = clientRect.top + (int)clientHeight;
	}

	// Calculate the outer dimensions of the physical window, including frame et. al.
	RECT windowRect = clientRect;
	AdjustWindowRectEx(&windowRect, windowStyleFlags, FALSE, windowStyleExFlags);

	WCHAR windowTitle[1024];
	MultiByteToWideChar(GetACP(), 0, m_config.m_windowTitle.c_str(), -1, windowTitle, sizeof(windowTitle) / sizeof(windowTitle[0]));
	HWND windowHandle = CreateWindowEx(
		windowStyleExFlags,
		windowClassDescription.lpszClassName,
		windowTitle,
		windowStyleFlags,
		windowRect.left,
		windowRect.top,
		windowRect.right - windowRect.left,
		windowRect.bottom - windowRect.top,
		NULL,
		NULL,
		(HINSTANCE)NULL,
		NULL); 
	ShowWindow(windowHandle, SW_SHOW);
	SetForegroundWindow(windowHandle);
	SetFocus(windowHandle);

	m_displayContext = GetDC(windowHandle);
	m_hwnd = windowHandle;
	HCURSOR cursor = LoadCursor(NULL, IDC_ARROW);
	SetCursor(cursor);
}

void Window::RunMessagePump()
{
	MSG queuedMessage;
	for (;; )
	{
		const BOOL wasMessagePresent = PeekMessage(&queuedMessage, NULL, 0, 0, PM_REMOVE);
		if (!wasMessagePresent)
		{
			break;
		}

		TranslateMessage(&queuedMessage);
		DispatchMessage(&queuedMessage); // This tells Windows to call our "WindowsMessageHandlingProcedure" (a.k.a. "WinProc") function
	}
}
