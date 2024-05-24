#include "Engine/UI/Button.hpp"
#include "Engine/Core/Vertex_PCU.hpp"
#include "Engine/Core/VertexUtils.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Renderer/BitmapFont.hpp"
#include "Engine/Renderer/Renderer.hpp"
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Input/InputSystem.hpp"

Button::Button(std::string text, AABB2 bounds, BitmapFont* font, Rgba8 textColor)
{
	m_bounds = bounds;
	m_text = text;
	m_textColor = textColor;
	m_textCellHeight = bounds.GetDimensions().y * 0.6f;
	m_font = font;
}

Button::~Button()
{
}


bool Button::IsPositionInside(Vec2 position)
{
	if (GetNearestPointOnAABB2D(position, m_bounds) == position)
		return true;
	return false;
}

void Button::AddVertsForButtonBackground(std::vector<Vertex_PCU>& verts) const
{
	if (m_isSelected) {
		Vec2 dimension = m_bounds.GetDimensions();
		Vec2 smaller = Vec2(dimension.x - 3.f, dimension.y - 3.f);
		AABB2 smallerBounds = m_bounds;
		smallerBounds.SetDimensions(smaller);

		AddVertsForAABB2D(verts, m_bounds, m_normalColor);
		AddVertsForAABB2D(verts, smallerBounds, m_selectColor);
	}
	else if (m_isHovered) {
		Vec2 dimension = m_bounds.GetDimensions();
		Vec2 smaller = Vec2(dimension.x - 3.f, dimension.y - 3.f);
		AABB2 smallerBounds = m_bounds;
		smallerBounds.SetDimensions(smaller);

		AddVertsForAABB2D(verts, m_bounds, m_normalColor);
		AddVertsForAABB2D(verts, smallerBounds, m_hoverColor);
	}
	else {
		AddVertsForAABB2D(verts, m_bounds, m_normalColor);
	}
}

void Button::AddVertsForButtonText(std::vector<Vertex_PCU>& verts) const
{
	if (m_isHovered) {
		m_font->AddVertsForTextInBox2D(verts, m_bounds, m_textCellHeight, m_text, m_hoverText, 0.7f, m_alignment);
	}
	else if (m_isSelected) {
		m_font->AddVertsForTextInBox2D(verts, m_bounds, m_textCellHeight, m_text, m_selectText, 0.7f, m_alignment);
	}
	else {
		m_font->AddVertsForTextInBox2D(verts, m_bounds, m_textCellHeight, m_text, m_normalText, 0.7f, m_alignment);
	}
}

void Button::Update(Vec2 position)
{
	m_isHovered = false;
	m_isSelected = false;
	if (IsPositionInside(position)) {
		m_isHovered = true;
		if (g_theInput->WasKeyJustPressed(KEYCODE_LEFT_MOUSE)) {
			m_isSelected = true;
		}
	}
}

void Button::Render() const
{
	std::vector<Vertex_PCU> bg, text;
	AddVertsForButtonBackground(bg);
	AddVertsForButtonText(text);

	g_theRenderer->DrawVertexArray(int(bg.size()), bg.data());
	g_theRenderer->BindTexture(&m_font->GetTexture());
	g_theRenderer->DrawVertexArray(int(text.size()), text.data());
	g_theRenderer->BindTexture(nullptr);
}
