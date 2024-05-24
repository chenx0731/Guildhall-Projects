#include "Game/Player.hpp"
#include "Game/GameCommon.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Math/Mat44.hpp"

Player::Player(Game* owner, Vec3 const& startPos) : Entity(owner, startPos)
{
	m_game = owner;
	m_position = startPos;
	m_camera.SetPerspectiveView(2.f, 60.f, 0.1f, 1000.f);
	m_camera.SetRenderBasis(Vec3(0.f, 0.f, 1.f), Vec3(-1.f, 0.f, 0.f), Vec3(0.f, 1.f, 0.f));
}

Player::~Player()
{
}

void Player::Update()
{
	Vec3 x_forward;
	Vec3 y_left;
	Vec3 z_up;
	m_orientation.m_yawDegrees -= g_theInput->GetCursorClientDelta().x * 0.5f;
	m_orientation.m_pitchDegrees += g_theInput->GetCursorClientDelta().y * 0.5f;
	m_orientation.m_pitchDegrees = GetClamped(m_orientation.m_pitchDegrees, -85.f, 85.f);

	m_orientation.GetVectors_XFwd_YLeft_ZUp(x_forward, y_left, z_up);

	XboxController tempController = g_theInput->GetController(0);
	Vec3 direction;

	//if (g_theInput->IsKeyDown('E') || tempController.GetRightTrigger() > 0.f) {
		//m_orientation.m_rollDegrees += m_game->m_clock->GetDeltaSeconds() * 20.f;
	//}
	if (g_theInput->IsKeyDown('S') || tempController.GetLeftStick().GetPosition().y < 0) {
		//m_position.x -= m_game->m_clock->GetDeltaSeconds() * 2.f;
		Vec3 forward = x_forward;
		forward.z = 0.f;
		//m_position -= m_game->m_clock->GetDeltaSeconds() * m_cameraSpeed * forward;
		direction -= forward;
	}
	//if (g_theInput->IsKeyDown('Q') || tempController.GetLeftTrigger() > 0.f) {
		//m_orientation.m_rollDegrees -= m_game->m_clock->GetDeltaSeconds() * 20.f;
	//}
	if (g_theInput->IsKeyDown('W') || tempController.GetLeftStick().GetPosition().y > 0) {
		//m_position.x += m_game->m_clock->GetDeltaSeconds() * 2.f;
		Vec3 forward = x_forward;
		forward.z = 0.f;
		//m_position += m_game->m_clock->GetDeltaSeconds() * m_cameraSpeed * forward;
		direction += forward;
	}
	if (g_theInput->IsKeyDown('A') || tempController.GetLeftStick().GetPosition().x < 0) {
		//m_position.y += m_game->m_clock->GetDeltaSeconds() * 2.f;
		Vec3 left = y_left;
		left.z = 0.f;
		direction += left;
		//m_position += m_game->m_clock->GetDeltaSeconds() * m_cameraSpeed * left;
	}
	if (g_theInput->IsKeyDown('D') || tempController.GetLeftStick().GetPosition().x > 0) {
		Vec3 left = y_left;
		left.z = 0.f;
		direction -= left;
		//m_position.y -= m_game->m_clock->GetDeltaSeconds() * 2.f;
		//m_position -= m_game->m_clock->GetDeltaSeconds() * m_cameraSpeed * left;
	}
	if (g_theInput->IsKeyDown('Q') || tempController.GetButton(XBOX_BUTTON_L_SHOULDER).IsKeyDown()) {
		//m_position.y += m_game->m_clock->GetDeltaSeconds() * 2.f;
		//m_position += m_game->m_clock->GetDeltaSeconds() * m_cameraSpeed * Vec3(0.f, 0.f, 1.f);
		direction += Vec3(0.f, 0.f, 1.f);
	}
	if (g_theInput->IsKeyDown('E') || tempController.GetButton(XBOX_BUTTON_R_SHOULDER).IsKeyDown()) {

		direction -= Vec3(0.f, 0.f, 1.f);
		//m_position -= m_game->m_clock->GetDeltaSeconds() * m_cameraSpeed * Vec3(0.f, 0.f, 1.f);
	}
	if (g_theInput->IsKeyDown('H') || tempController.GetButton(XBOX_BUTTON_START).IsKeyDown()) {

		m_position = Vec3(0.f, 0.f, 0.f);
		m_orientation.m_yawDegrees = 0.f;
		m_orientation.m_pitchDegrees = 0.f;
		m_orientation.m_rollDegrees = 0.f;
	}
	if (g_theInput->IsKeyDown(' ') || tempController.GetButton(XBOX_BUTTON_A).IsKeyDown()) {
		m_cameraSpeed = 40.f;
	}
	if (g_theInput->WasKeyJustReleased(' ') || tempController.GetButton(XBOX_BUTTON_A).WasKeyJustReleased()) {
		m_cameraSpeed = 4.f;
	}
	if (g_theInput->IsKeyDown(KEYCODE_UPARROW) || tempController.GetRightStick().GetPosition().y > 0) {
		m_orientation.m_pitchDegrees -= m_game->m_clock->GetDeltaSeconds() * 40.f;
	}
	if (g_theInput->IsKeyDown(KEYCODE_DOWNARROW) || tempController.GetRightStick().GetPosition().y < 0) {
		m_orientation.m_pitchDegrees += m_game->m_clock->GetDeltaSeconds() * 40.f;
	}
	if (g_theInput->IsKeyDown(KEYCODE_RIGHTARROW) || tempController.GetRightStick().GetPosition().x > 0) {
		m_orientation.m_yawDegrees -= m_game->m_clock->GetDeltaSeconds() * 40.f;
	}
	if (g_theInput->IsKeyDown(KEYCODE_LEFTARROW) || tempController.GetRightStick().GetPosition().x < 0) {
		m_orientation.m_yawDegrees += m_game->m_clock->GetDeltaSeconds() * 40.f;
	}
	m_position += m_game->m_clock->GetDeltaSeconds() * m_cameraSpeed * direction.GetNormalized();
	m_orientation.m_rollDegrees = GetClamped(m_orientation.m_rollDegrees, -45.f, 45.f);
	m_camera.SetTransform(m_position, m_orientation);
}

void Player::Render() const
{
}

Vec3 Player::GetFwdNormal() const
{
	Vec3 fwdNormal;
	fwdNormal.x = CosDegrees(m_orientation.m_yawDegrees) * CosDegrees(m_orientation.m_pitchDegrees);
	fwdNormal.y = SinDegrees(m_orientation.m_yawDegrees) * CosDegrees(m_orientation.m_pitchDegrees);
	fwdNormal.z = -SinDegrees(m_orientation.m_pitchDegrees);
	return fwdNormal;
}

