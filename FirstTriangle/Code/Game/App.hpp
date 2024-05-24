#pragma once
#include "Engine/Math/Vec2.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Renderer/Window.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
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


private:
	void BeginFrame();
	void Update(float deltaSeconds);
	void Render() const;
	void EndFrame();

	void UpdateFromKeyboard(float deltaSeconds);

private:

	bool m_isAttractMode = true;
	bool m_isQuitting = false;
	bool m_isPaused = false;
	bool m_isSlowMo = false;

};