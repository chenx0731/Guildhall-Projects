#pragma once
#include "Engine/UI/ImGuiBase.hpp"
#include <string>

class SRTEditor : public ImGuiBase
{
public:
	SRTEditor(ImGuiBaseConfig const& config);
	~SRTEditor();

	virtual void Update() override;
public:
	std::string m_currentEffectName;
};

