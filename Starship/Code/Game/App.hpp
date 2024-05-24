#pragma once
#include "Engine/Math/Vec2.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Audio/AudioSystem.hpp"
#include "Engine/Renderer/Window.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Math/RandomNumberGenerator.hpp"
#include "Game/Game.hpp"
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

	void SetGameTimeScale(float scale);
private:
	void BeginFrame();
	void Update();
	void Render() const;
	void EndFrame();

	void UpdateFromKeyboard();

private:
	Game* m_theGame;

	bool m_isAttractMode = true;
	bool m_isQuitting = false;
	bool m_isPaused = false;
	bool m_isSlowMo = false;

	Camera* m_devCamera;

};

void ShowAllKeysOnDevConsole();