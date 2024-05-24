#pragma once
#include "Engine/Math/Vec2.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Renderer/Window.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Game/Game.hpp"

class TestForEventRecipient : public EventRecipient
{
public:
	TestForEventRecipient();
	~TestForEventRecipient();

	bool TestEventRecipient01(EventArgs& args) { return false; };
	bool TestEventRecipient02(EventArgs& args) { return false; };
	bool TestEventRecipient03(EventArgs& args) { return false; };
};

class App
{
public:
	App();
	~App();
	void Startup();
	void Shutdown();
	void RunFrame();
	void Run();

	bool IsQuitting() const { return m_isQuitting; }
	bool HandleQuitRequested();

	bool Event_Quit(EventArgs& args);

private:
	void BeginFrame();
	void Update();
	void Render() const;
	void EndFrame();

	void LoadAssets();
	void UpdateFromKeyboard();
private:
	bool m_isAttractMode = false;
	bool m_isQuitting = false;
	bool m_isPaused = false;
	bool m_isSlowMo = false;

	Camera* m_devCamera = nullptr;

};