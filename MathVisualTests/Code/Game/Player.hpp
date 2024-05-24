#pragma once
#include "Game/Entity.hpp"
#include "Engine/Renderer/Camera.hpp"


class Player : public Entity
{
public:
	Player(Game* owner, Vec3 const& startPos);
	~Player();
	virtual void Update(float deltaSecond) override;
	virtual void Render() const override;

public:
	Camera m_camera;
};