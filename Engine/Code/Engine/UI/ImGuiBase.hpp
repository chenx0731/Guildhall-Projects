#pragma once

class Window;
class Renderer;

struct ImGuiBaseConfig
{
	Window*		m_window;
	Renderer*	m_renderer;
};


class ImGuiBase
{
public:
	ImGuiBase(ImGuiBaseConfig const& config);
	~ImGuiBase();
	
	void Startup();
	void Shutdown();
	void BeginFrame();
	void EndFrame();

	virtual void Update();
	void Render() const;

public:
	ImGuiBaseConfig	m_config;
	bool				m_isShowDemo;
};

