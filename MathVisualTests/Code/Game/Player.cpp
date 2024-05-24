#include "Game/Player.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Math/Mat44.hpp"

Player::Player(Game* owner, Vec3 const& startPos) : Entity(owner, startPos)
{
	m_game = owner;
	m_position = startPos;
	m_camera.SetPerspectiveView(2.f, 60.f, 0.1f, 100.f);
	m_camera.SetRenderBasis(Vec3(0.f, 0.f, 1.f), Vec3(-1.f, 0.f, 0.f), Vec3(0.f, 1.f, 0.f));
}

Player::~Player()
{
}

void Player::Update(float deltaSecond)
{
	Vec3 x_forward;
	Vec3 y_left;
	Vec3 z_up;
	m_orientation.m_yawDegrees -= g_theInput->GetCursorClientDelta().x * 0.5f;
	m_orientation.m_pitchDegrees -= g_theInput->GetCursorClientDelta().y * 0.5f;
	m_orientation.m_pitchDegrees = GetClamped(m_orientation.m_pitchDegrees, -89.9f, 89.9f);
	//m_orientation.m_yawDegrees
	m_orientation.GetVectors_XFwd_YLeft_ZUp(x_forward, y_left, z_up);
	if (g_theInput->IsKeyDown('E')) {

		//m_position.z += m_game->m_clock->GetDeltaSeconds() * 2.f;
		m_position += deltaSecond * 5.f * z_up;
	}
	if (g_theInput->IsKeyDown('S')) {
		//m_position.x -= m_game->m_clock->GetDeltaSeconds() * 2.f;
		m_position -= deltaSecond * 5.f * x_forward;
	}
	if (g_theInput->IsKeyDown('Q')) {
		//m_position.z -= m_game->m_clock->GetDeltaSeconds() * 2.f;
		m_position -= deltaSecond * 5.f * z_up;
	}
	if (g_theInput->IsKeyDown('W')) {
		//m_position.x += m_game->m_clock->GetDeltaSeconds() * 2.f;
		m_position += deltaSecond * 5.f * x_forward;
	}
	if (g_theInput->IsKeyDown('A')) {
		//m_position.y += m_game->m_clock->GetDeltaSeconds() * 2.f;
		m_position += deltaSecond * 5.f * y_left;
	}
	if (g_theInput->IsKeyDown('D')) {
		//m_position.y -= m_game->m_clock->GetDeltaSeconds() * 2.f;
		m_position -= deltaSecond * 5.f * y_left;
	}
	if (g_theInput->IsKeyDown(KEYCODE_UPARROW)) {
		m_orientation.m_pitchDegrees -= deltaSecond * 40.f;
	}
	if (g_theInput->IsKeyDown(KEYCODE_DOWNARROW)) {
		m_orientation.m_pitchDegrees += deltaSecond * 40.f;
	}
	if (g_theInput->IsKeyDown(KEYCODE_RIGHTARROW)) {
		m_orientation.m_yawDegrees -= deltaSecond * 40.f;
	}
	if (g_theInput->IsKeyDown(KEYCODE_LEFTARROW)) {
		m_orientation.m_yawDegrees += deltaSecond * 40.f;
	}
	m_camera.SetTransform(m_position, m_orientation);
}

void Player::Render() const
{
}

