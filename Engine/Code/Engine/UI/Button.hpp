#pragma once
#include "Engine/Math/AABB2.hpp"
#include "Engine/Core/Rgba8.hpp"
#include "Engine/Renderer/SpriteAnimDefinition.hpp"
#include <vector>

struct Vertex_PCU;
class BitmapFont;

class Button
{
public:
	Button(std::string text, AABB2 bounds, BitmapFont* font, Rgba8 textColor = Rgba8::BLACK);
	~Button();


	bool IsPositionInside(Vec2 position);
	void AddVertsForButtonBackground(std::vector<Vertex_PCU>& verts) const;
	void AddVertsForButtonText(std::vector<Vertex_PCU>& verts) const;

	void Update(Vec2 position);
	void Render() const;

public:
	AABB2 m_bounds;
	std::string m_text;
	BitmapFont* m_font = nullptr;

	Rgba8 m_normalColor = Rgba8::BLACK;
	Rgba8 m_hoverColor = Rgba8::WHITE;
	Rgba8 m_selectColor = Rgba8::WHITE;

	Rgba8 m_normalText = Rgba8::WHITE;
	Rgba8 m_hoverText = Rgba8::BLACK;
	Rgba8 m_selectText = Rgba8::BLACK;

	Rgba8 m_textColor;

	Vec2 m_alignment = Vec2(0.01f, 0.5f);

	bool m_isHovered;
	bool m_isSelected;
	bool m_isFocused;

	float m_textCellHeight = 0.f;
};