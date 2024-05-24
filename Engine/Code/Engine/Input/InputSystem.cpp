#include "Engine/Input/InputSystem.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Renderer/Window.hpp"
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

const unsigned char KEYCODE_F1				= VK_F1;
const unsigned char KEYCODE_F2				= VK_F2;
const unsigned char KEYCODE_F3				= VK_F3;
const unsigned char KEYCODE_F4				= VK_F4;
const unsigned char KEYCODE_F5				= VK_F5;
const unsigned char KEYCODE_F6				= VK_F6;
const unsigned char KEYCODE_F7				= VK_F7;
const unsigned char KEYCODE_F8				= VK_F8;
const unsigned char KEYCODE_F9				= VK_F9;
const unsigned char KEYCODE_F10				= VK_F10;
const unsigned char KEYCODE_F11				= VK_F11;
const unsigned char KEYCODE_ESC				= VK_ESCAPE;
const unsigned char KEYCODE_SPACE			= VK_SPACE;
const unsigned char KEYCODE_ENTER			= VK_RETURN;
const unsigned char KEYCODE_UPARROW			= VK_UP;
const unsigned char KEYCODE_DOWNARROW		= VK_DOWN;
const unsigned char KEYCODE_LEFTARROW		= VK_LEFT;
const unsigned char KEYCODE_RIGHTARROW		= VK_RIGHT;
const unsigned char KEYCODE_LEFT_MOUSE		= VK_LBUTTON;
const unsigned char KEYCODE_RIGHT_MOUSE		= VK_RBUTTON;
const unsigned char KEYCODE_TILDE			= 0xC0;
const unsigned char KEYCODE_BACKSPACE		= VK_BACK;
const unsigned char KEYCODE_INSERT			= VK_INSERT;
const unsigned char KEYCODE_DELETE			= VK_DELETE;
const unsigned char KEYCODE_HOME			= VK_HOME;
const unsigned char KEYCODE_END				= VK_END;
const unsigned char KEYCODE_SHIFT			= VK_SHIFT;
const unsigned char KEYCODE_LEFTBRACKET		= 0xDB;
const unsigned char KEYCODE_RIGHTBRACKET	= 0xDD;
const unsigned char KEYCODE_COMMA			= VK_OEM_COMMA;
const unsigned char KEYCODE_PERIOD			= VK_OEM_PERIOD;
const unsigned char KEYCODE_SEMICOLON		= VK_OEM_1;
const unsigned char KEYCODE_SINGLEQUOTE		= VK_OEM_7;

InputSystem* InputSystem::s_theInput = nullptr;

InputSystem::InputSystem(InputSystemConfig const& config) : m_config(config)
{
	s_theInput = this;
	
}

InputSystem::~InputSystem()
{
}

void InputSystem::Startup()
{
	for (int controllerIndex = 0; controllerIndex < NUM_XBOX_CONTROLLERS; controllerIndex++)
	{
		m_controllers[controllerIndex].m_id = controllerIndex;
	}
	g_theEventSystem->SubscribeEventCallbackFunction("KeyPressed", InputSystem::Event_KeyPressed);
	g_theEventSystem->SubscribeEventCallbackFunction("KeyReleased", InputSystem::Event_KeyReleased);
}

void InputSystem::Shutdown()
{

}

void InputSystem::BeginFrame()
{
	for (int controllerIndex = 0; controllerIndex < NUM_XBOX_CONTROLLERS; controllerIndex++)
	{
		m_controllers[controllerIndex].Update();
	}

	//if (Window::s_mainWindow->IsFocused())

	if (m_mouseState.m_currentHidden != m_mouseState.m_desiredHidden) {
		m_mouseState.m_currentHidden = m_mouseState.m_desiredHidden;
		if (m_mouseState.m_currentHidden) {
			while (::ShowCursor(false) >= 0) {
				
			}
		}
		else {
			while (::ShowCursor(true) < 0) {
				
			}
		}
		//m_mouseState.m_currentHidden = m_mouseState.m_desiredHidden;
	}
	//::ShowCursor(m_mouseState.m_currentHidden);
	HWND windowHandle = (HWND)(Window::GetWindowContext()->GetHwnd());
	//= HWND(m_hwnd);
	POINT cursorCoords;
	RECT clientRect;
	::GetCursorPos(&cursorCoords);
	::ScreenToClient(windowHandle, &cursorCoords);
	::GetClientRect(windowHandle, &clientRect);
	POINT clientCenter;
	clientCenter.x = (LONG)((clientRect.right - clientRect.left) * (float)0.5f);
	clientCenter.y = (LONG)((clientRect.bottom - clientRect.top) * (float)0.5f);
	IntVec2 currentPos = IntVec2(cursorCoords.x, cursorCoords.y);
	
	//Window* currentWindow = Window::GetWindowContext();
	//Vec2 normalized = GetCursorNormalizedPosition();
	//Vec2 pos = Vec2(normalized.x * (float)currentWindow->GetClientDimensions().x, normalized.y * (float)currentWindow->GetClientDimensions().y);
	//currentPos = IntVec2((int)pos.x, (int)pos.y);

	//m_mouseState.m_cursorClientPosition = IntVec2(cursorCoords.x, cursorCoords.y)

	if (m_mouseState.m_currentRelative != m_mouseState.m_desiredRelative) {
		m_mouseState.m_currentRelative = m_mouseState.m_desiredRelative;
		m_mouseState.m_cursorClientPosition = IntVec2((int)clientCenter.x, (int)clientCenter.y);
		m_mouseState.m_cursorClientDelta = IntVec2::Zero;

		cursorCoords.x = clientCenter.x;
		cursorCoords.y = clientCenter.y;

		::ClientToScreen(windowHandle, &clientCenter);
		::SetCursorPos(clientCenter.x, clientCenter.y);

		::GetCursorPos(&clientCenter);
		::ScreenToClient(windowHandle, &clientCenter);

		m_mouseState.m_cursorClientPosition = IntVec2((int)clientCenter.x, (int)clientCenter.y);
		//currentPos = m_mouseState.m_cursorClientPosition;
	}

	if (m_mouseState.m_currentRelative) {
		//::GetCursorPos(&cursorCoords);
		//currentPos = IntVec2(cursorCoords.x, cursorCoords.y);
		m_mouseState.m_cursorClientDelta = currentPos - m_mouseState.m_cursorClientPosition;
		
		::ClientToScreen(windowHandle, &clientCenter);
		::SetCursorPos(clientCenter.x, clientCenter.y);
		::GetCursorPos(&cursorCoords);
		::ScreenToClient(windowHandle, &cursorCoords);

		m_mouseState.m_cursorClientPosition.x = cursorCoords.x;
		m_mouseState.m_cursorClientPosition.y = cursorCoords.y;
	}

	//GetCursorPos(&cursorCoords);
	//currentPos = IntVec2(cursorCoords.x, cursorCoords.y);
	
	//m_mouseState.m_cursorClientPosition = currentPos;
	

}

void InputSystem::EndFrame()
{
	for (int keyIndex = 0; keyIndex < NUM_KEYCODES; keyIndex++)
	{
		m_keyStates[keyIndex].UpdateKeyDownLastFrame();
	}
}

bool InputSystem::WasKeyJustPressed(unsigned char keyCode)
{
	return m_keyStates[keyCode].WasKeyJustPressed();
}

bool InputSystem::WasKeyJustReleased(unsigned char keyCode)
{
	return m_keyStates[keyCode].WasKeyJustReleased();
}

bool InputSystem::IsKeyDown(unsigned char keyCode)
{
	return m_keyStates[keyCode].IsKeyDown();
}

void InputSystem::HandleKeyPressed(unsigned char keyCode)
{
	m_keyStates[keyCode].m_keyDownThisFrame = true;
}

void InputSystem::HandleKeyReleased(unsigned char keyCode)
{
	m_keyStates[keyCode].m_keyDownThisFrame = false;
}

XboxController const& InputSystem::GetController(int controllerID)
{
	return m_controllers[controllerID];
}

InputSystemConfig const& InputSystem::GetConfig() const
{
	// TODO: insert return statement here
	return m_config;
}

void InputSystem::SetCursorMode(bool hidden, bool relative)
{
	m_mouseState.m_desiredHidden = hidden;
	m_mouseState.m_desiredRelative = relative;
	if (m_mouseState.m_currentHidden != m_mouseState.m_desiredHidden) {
		m_mouseState.m_currentHidden = m_mouseState.m_desiredHidden;
		if (m_mouseState.m_currentHidden) {
			while (::ShowCursor(false) >= 0) {

			}
		}
		else {
			while (::ShowCursor(true) < 0) {

			}
		}
		//m_mouseState.m_currentHidden = m_mouseState.m_desiredHidden;
	}
}

Vec2 InputSystem::GetCursorClientDelta() const
{	
	return Vec2((float)m_mouseState.m_cursorClientDelta.x, (float)m_mouseState.m_cursorClientDelta.y);
}

Vec2 InputSystem::GetCursorClientPosition() const
{
	return Vec2((float)m_mouseState.m_cursorClientPosition.x, (float)m_mouseState.m_cursorClientPosition.y);
}

Vec2 InputSystem::GetCursorNormalizedPosition() const
{
	//HWND windowHandle = ::GetActiveWindow();
	////= HWND(m_hwnd);
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
	float clientHeight = float(clientRect.bottom);
	return Vec2(mouseClientX / clientWidth, 1.f - (mouseClientY / clientHeight));
}

MouseState InputSystem::GetMouseState() const
{
	return m_mouseState;
}

bool InputSystem::Event_KeyPressed(EventArgs& args)
{
	if (!s_theInput)
	{
		return false;
	}
	unsigned char keyCode = (unsigned char)args.GetValue("KeyCode", -1);
	s_theInput->HandleKeyPressed(keyCode);
	return true;
}

bool InputSystem::Event_KeyReleased(EventArgs& args)
{
	if (!s_theInput)
	{
		return false;
	}
	unsigned char keyCode = (unsigned char)args.GetValue("KeyCode", -1);
	s_theInput->HandleKeyReleased(keyCode);
	return true;
}
