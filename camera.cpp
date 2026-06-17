#include "camera.h"

Camera::Camera()
{
	view = View(Vector2f(SCREEN_X / 2, SCREEN_Y / 2), Vector2f(SCREEN_X, SCREEN_Y));
	posX = 0;
	posY = 0;
}

void Camera::update(Vector2f playerPos)
{
	float targetX = playerPos.x + 36;
	float targetY = SCREEN_Y / 2;

	if (targetX < SCREEN_X / 2)
		targetX = SCREEN_X / 2;
	if (targetX > WORLD_WIDTH - SCREEN_X / 2)
		targetX = WORLD_WIDTH - SCREEN_X / 2;

	posX = targetX;
	posY = targetY;

	view.setCenter(posX, posY);
}

View Camera::getView() const { return view; }
float Camera::getCameraX() const { return posX - SCREEN_X / 2; }
float Camera::getCameraY() const { return posY; }
