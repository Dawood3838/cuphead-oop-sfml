#ifndef PROJECTILE_H
#define PROJECTILE_H

#include <SFML/Graphics.hpp>
#include "constants.h"

using namespace sf;

// ==================== PROJECTILE CLASS ====================
class Projectile
{
private:
	Sprite sprite;
	Texture texture;
	float velocityX;
	float velocityY;
	bool active;
	int damage;
	WeaponType weaponType;
	float lifetime;
	float maxLifetime;
	Color projectileColor;
	float size;
	bool parryable;  // Pink objects that can be parried

public:
	Projectile();

	void initialize(float x, float y, float dirX, float dirY, WeaponType weapon, int dmg);
	void update();
	void draw(RenderWindow& window);

	bool isActive() const;
	void setInactive();
	FloatRect getBounds() const;
	int getDamage() const;
	WeaponType getWeaponType() const;
	bool getParryable() const;
	Vector2f getPosition() const { return sprite.getPosition(); }
};

#endif // PROJECTILE_H
