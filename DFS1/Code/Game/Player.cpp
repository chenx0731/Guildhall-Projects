#include "Game/Player.hpp"
#include "Game/Actor.hpp"
#include "Game/ActorDefinition.hpp"
#include "Game/Map.hpp"
#include "Game/GameCommon.hpp"
#include "Game/GamePlaying.hpp"
#include "Game/Weapon.hpp"
#include "Engine/Input/InputSystem.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/Clock.hpp"
#include "Engine/Math/Mat44.hpp"
#include "Engine/Math/RaycastResult2D.hpp"
#include "Engine/Core/DebugRenderSystem.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"


Player::Player(Map* map) : Controller(map)
{
	m_worldCamera.SetPerspectiveView(2.f, 60.f, 0.1f, 100.f);
	m_worldCamera.SetRenderBasis(Vec3(0.f, 0.f, 1.f), Vec3(-1.f, 0.f, 0.f), Vec3(0.f, 1.f, 0.f));
}

Player::~Player()
{
	//Actor* actor = GetActor();
	//if (actor != nullptr) {
	//	ActorUID uid = actor->m_UID;
	//	delete actor->m_map->m_actors[uid.GetIndex()];
	//	actor->m_map->m_actors[uid.GetIndex()] = nullptr;
	//}
}

void Player::Update(float deltaSeconds)
{
	UpdateCamera();
	UpdateInput(deltaSeconds);
	
}

void Player::UpdateInput(float deltaSeconds)
{
	Vec3 x_forward;
	Vec3 y_left;
	Vec3 z_up;

	m_orientation.GetVectors_XFwd_YLeft_ZUp(x_forward, y_left, z_up);

	XboxController tempController;
	if (m_type == ControllerType::CONTROLLER0)
		tempController = g_theInput->GetController(0);
	if (m_type == ControllerType::CONTROLLER1)
		tempController = g_theInput->GetController(1);

	float conLeftStickX = tempController.GetLeftStick().GetPosition().x;
	float conLeftStickY = tempController.GetLeftStick().GetPosition().y;
	float conRightStickX = tempController.GetRightStick().GetPosition().x;
	float conRightStickY = tempController.GetRightStick().GetPosition().y;


	if (g_theInput->WasKeyJustPressed('N')) {
		GamePlaying* playing = (GamePlaying*)g_theGames[(int)GameState::PLAYING];
		if (playing->m_playerNum == 1)
			m_map->DebugPossessNext();
	}
	if (g_theInput->WasKeyJustPressed('F')) {
		ToggleCameraMode();
	}
	
	Actor* actor = GetActor();
	if (g_theInput->IsKeyDown(KEYCODE_LEFT_MOUSE)) {
		if (actor->m_weapons.size() > 0) {
			actor->m_weapons[actor->m_currentWeaponIndex]->Fire(actor);
		}
	}
	if (tempController.GetRightTrigger() > 0.2f) {
		if (actor->m_weapons.size() > 0) {
			actor->m_weapons[actor->m_currentWeaponIndex]->Fire(actor);
		}
	}
	if (actor) {
		if (actor->m_isRiding && actor->m_ridingActor) {
			actor = actor->m_ridingActor;
		}
	}
	

	if (actor != nullptr && m_cameraMode == CameraMode::First_Person) {
		m_position = actor->m_position;
	}
	if (m_type == ControllerType::KEYBOARD) {
		m_orientation.m_yawDegrees -= g_theInput->GetMouseState().m_cursorClientDelta.x * 0.075f; //* deltaSeconds;
			//g_theInput->GetCursorClientDelta().x * deltaSeconds * 5.f;
		m_orientation.m_pitchDegrees += g_theInput->GetMouseState().m_cursorClientDelta.y * 0.075f; // *deltaSeconds;
			//g_theInput->GetCursorClientDelta().y * deltaSeconds * 5.f;

		m_orientation.m_pitchDegrees = GetClamped(m_orientation.m_pitchDegrees, -85.f, 85.f);
		//if (g_theInput->IsKeyDown(KEYCODE_LEFT_MOUSE)) {
		//	if (actor->m_weapons.size() > 0) {
		//		actor->m_weapons[actor->m_currentWeaponIndex]->Fire(actor);
		//	}
		//}

		m_cameraSpeed = actor->m_actorDef->m_walkSpeed;

		if (g_theInput->IsKeyDown(KEYCODE_SHIFT)) {
			m_cameraSpeed = actor->m_actorDef->m_runSpeed;
		}
		if (g_theInput->WasKeyJustReleased(KEYCODE_SHIFT)) {
			m_cameraSpeed = actor->m_actorDef->m_walkSpeed;
		}

		if (actor->m_isRiding) {
			m_cameraSpeed *= 3.f;
			GetClamped(m_cameraSpeed, 0.f, 8.f);
		}

		if (g_theInput->WasKeyJustPressed('1')) {
			actor->m_currentWeaponIndex = actor->m_currentWeaponIndex - 1 < 0 ? ((int)actor->m_weapons.size() - 1) : (actor->m_currentWeaponIndex - 1);
		}
		if (g_theInput->WasKeyJustPressed('2')) {
			actor->m_currentWeaponIndex = actor->m_currentWeaponIndex + 1 >= (int)actor->m_weapons.size() ?
				0 : actor->m_currentWeaponIndex + 1;
		}

		if (g_theInput->IsKeyDown('S')) {
			//m_position.x -= m_game->m_clock->GetDeltaSeconds() * 2.f;
			m_position -= deltaSeconds * m_cameraSpeed * x_forward;
		}
		if (g_theInput->IsKeyDown('W')) {
			//m_position.x += m_game->m_clock->GetDeltaSeconds() * 2.f;
			m_position += deltaSeconds * m_cameraSpeed * x_forward;
		}
		if (g_theInput->IsKeyDown('A')) {
			//m_position.y += m_game->m_clock->GetDeltaSeconds() * 2.f;
			m_position += deltaSeconds * m_cameraSpeed * y_left;
		}
		if (g_theInput->IsKeyDown('D')) {
			//m_position.y -= m_game->m_clock->GetDeltaSeconds() * 2.f;
			m_position -= deltaSeconds * m_cameraSpeed * y_left;
		}
		if (g_theInput->IsKeyDown('Z')) {
			//m_position.y += m_game->m_clock->GetDeltaSeconds() * 2.f;
			m_position += deltaSeconds * m_cameraSpeed * Vec3(0.f, 0.f, 1.f);
		}
		if (g_theInput->IsKeyDown('C')) {

			m_position -= deltaSeconds * m_cameraSpeed * Vec3(0.f, 0.f, 1.f);
		}

		if (g_theInput->IsKeyDown(KEYCODE_UPARROW)) {
			m_orientation.m_pitchDegrees -= deltaSeconds * 40.f;
		}
		if (g_theInput->IsKeyDown(KEYCODE_DOWNARROW)) {
			m_orientation.m_pitchDegrees += deltaSeconds * 40.f;
		}
		if (g_theInput->IsKeyDown(KEYCODE_RIGHTARROW)) {
			m_orientation.m_yawDegrees -= deltaSeconds * 40.f;
		}
		if (g_theInput->IsKeyDown(KEYCODE_LEFTARROW)) {
			m_orientation.m_yawDegrees += deltaSeconds * 40.f;
		}

	}
	else {
		/*if (tempController.GetRightTrigger() > 0.2f) {
			if (actor->m_weapons.size() > 0) {
				actor->m_weapons[actor->m_currentWeaponIndex]->Fire(actor);
			}
		}*/
	
		if (tempController.GetButton(XBOX_BUTTON_UP).WasKeyJustPressed()) {
			actor->m_currentWeaponIndex = actor->m_currentWeaponIndex - 1 < 0 ? ((int)actor->m_weapons.size() - 1) : (actor->m_currentWeaponIndex - 1);
		}
		if (tempController.GetButton(XBOX_BUTTON_DOWN).WasKeyJustPressed()) {
			actor->m_currentWeaponIndex = actor->m_currentWeaponIndex + 1 >= (int)actor->m_weapons.size() ?
				0 : actor->m_currentWeaponIndex + 1;
		}
	
		if ((conLeftStickY < 0 && fabs(conLeftStickY) > fabs(conLeftStickX))) {
			//m_position.x -= m_game->m_clock->GetDeltaSeconds() * 2.f;
			m_position -= deltaSeconds * m_cameraSpeed * x_forward;
		}
		if ( (conLeftStickY > 0 && fabs(conLeftStickY) > fabs(conLeftStickX))) {
			//m_position.x += m_game->m_clock->GetDeltaSeconds() * 2.f;
			m_position += deltaSeconds * m_cameraSpeed * x_forward;
		}
		if ((conLeftStickX < 0 && fabs(conLeftStickX) > fabs(conLeftStickY))) {
			//m_position.y += m_game->m_clock->GetDeltaSeconds() * 2.f;
			m_position += deltaSeconds * m_cameraSpeed * y_left;
		}
		if ((conLeftStickX > 0 && fabs(conLeftStickX) > fabs(conLeftStickY))) {
			//m_position.y -= m_game->m_clock->GetDeltaSeconds() * 2.f;
			m_position -= deltaSeconds * m_cameraSpeed * y_left;
		}
		if (tempController.GetButton(XBOX_BUTTON_L_SHOULDER).IsKeyDown()) {
			//m_position.y += m_game->m_clock->GetDeltaSeconds() * 2.f;
			m_position += deltaSeconds * m_cameraSpeed * Vec3(0.f, 0.f, 1.f);
		}
		if (tempController.GetButton(XBOX_BUTTON_R_SHOULDER).IsKeyDown()) {

			m_position -= deltaSeconds * m_cameraSpeed * Vec3(0.f, 0.f, 1.f);
		}

		if (tempController.GetButton(XBOX_BUTTON_A).IsKeyDown()) {
			m_cameraSpeed = actor->m_actorDef->m_runSpeed;
		}
		if (tempController.GetButton(XBOX_BUTTON_A).WasKeyJustReleased()) {
			m_cameraSpeed = actor->m_actorDef->m_walkSpeed;
		}
		if ((conRightStickY > 0 && fabs(conRightStickY) > fabs(conRightStickX))) {
			m_orientation.m_pitchDegrees -= deltaSeconds * 40.f;
		}
		if ((conRightStickY < 0 && fabs(conRightStickY) > fabs(conRightStickX))) {
			m_orientation.m_pitchDegrees += deltaSeconds * 40.f;
		}
		if ((conRightStickX > 0 && fabs(conRightStickX) > fabs(conRightStickY))) {
			m_orientation.m_yawDegrees -= deltaSeconds * 40.f;
		}
		if ((conRightStickX < 0 && fabs(conRightStickX) > fabs(conRightStickY))) {
			m_orientation.m_yawDegrees += deltaSeconds * 40.f;
		}
	}
	
	if (g_theInput->WasKeyJustPressed(' ')) {
		if (actor->IsOnGround())
			actor->AddImpulse(Vec3(0.f, 0.f, actor->m_actorDef->m_walkSpeed * 2.5f));
	}

	if (m_cameraMode == CameraMode::First_Person) {
		float level = actor->m_map->m_level;
		if (actor != nullptr) {
			if (actor->m_health <= 0) {
				m_position.z = Interpolate(actor->m_actorDef->m_eyeHeight + level, level, actor->m_corpseLifetime.GetElapsedFraction());
			}
			else
			{
				actor->m_position.x = m_position.x;
				actor->m_position.y = m_position.y;
				m_position.z = actor->m_position.z + actor->m_actorDef->m_eyeHeight;

				actor->m_orientation = m_orientation;
			}
			
		}
		else m_position.z = level;
	}
	
	
	m_orientation.m_rollDegrees = GetClamped(m_orientation.m_rollDegrees, -45.f, 45.f);

}

void Player::UpdateCamera()
{
	Actor* actor = GetActor();
	if (actor != nullptr) {
		m_worldCamera.SetPerspectiveView(m_worldCamera.m_aspect, actor->m_actorDef->m_cameraFOV, 0.1f, 100.f);
	}
	else {
		m_worldCamera.SetPerspectiveView(m_worldCamera.m_aspect, 60.f, 0.1f, 100.f);
	}
	m_worldCamera.SetRenderBasis(Vec3(0.f, 0.f, 1.f), Vec3(-1.f, 0.f, 0.f), Vec3(0.f, 1.f, 0.f));
	m_worldCamera.SetTransform(m_position, m_orientation);
}

void Player::ToggleCameraMode()
{
	if (m_cameraMode == CameraMode::First_Person)
		m_cameraMode = CameraMode::Free_Fly;
	else m_cameraMode = CameraMode::First_Person;
}

void Player::Possess(ActorUID uid)
{
	Controller::Possess(uid);
	Actor* actor = GetActor();
	m_position = actor->m_position;
	m_orientation = actor->m_orientation;
	m_cameraSpeed = actor->m_actorDef->m_walkSpeed;
}


