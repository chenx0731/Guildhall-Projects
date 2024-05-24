#pragma once
#include "Game/Controller.hpp"
#include "Engine/Renderer/Camera.hpp"
class Map;

enum class CameraMode
{
	First_Person,
	Free_Fly
};

class Player : public Controller
{
public:
	Player(Map* map);
	~Player();
	void Update(float deltaSeconds) override;
	void UpdateInput(float deltaSeconds);
	void UpdateCamera();
	void ToggleCameraMode();
	void Possess(ActorUID uid);

public:
	int m_playerIndex;

	ControllerType m_type;
	Camera m_worldCamera;
	Camera m_screenCamera;
	float m_cameraSpeed = 1.f;
	Vec3 m_position;
	EulerAngles m_orientation;
	CameraMode m_cameraMode = CameraMode::First_Person;
};