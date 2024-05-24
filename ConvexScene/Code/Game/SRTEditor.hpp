#pragma once
#include "Engine/UI/ImGuiBase.hpp"


class SRTEditor : public ImGuiBase
{
public:
	SRTEditor(ImGuiBaseConfig const& config);
	~SRTEditor();

	virtual void Update() override;

};

