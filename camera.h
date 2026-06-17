#ifndef CAMERA_H
#define CAMERA_H

#include <SFML/Graphics.hpp>
#include "constants.h"

using namespace sf;

// ==================== CAMERA CLASS ====================
class Camera
{
private:
	View view;
	float posX;
	float posY;

public:
	Camera();

	void update(Vector2f playerPos);

	View getView() const;
	float getCameraX() const;
	float getCameraY() const;
};

#endif // CAMERA_H
