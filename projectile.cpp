#include "projectile.h"
#include <cmath>

Projectile::Projectile() : velocityX(0), velocityY(0), active(false), damage(10), weaponType(PEASHOOTER),
                           lifetime(0), maxLifetime(10.0f), projectileColor(Color::Yellow), size(5.0f), parryable(false) {}

void Projectile::initialize(float x, float y, float dirX, float dirY, WeaponType weapon, int dmg)
{
	// Normalize direction
	float length = sqrt(dirX * dirX + dirY * dirY);
	if (length > 0)
	{
		dirX /= length;
		dirY /= length;
	}

	weaponType = weapon;
	damage = dmg;
	lifetime = 0;
	active = true;
	sprite.setPosition(x, y);
	
	// 30% chance for any projectile to be parryable (pink)
	parryable = (rand() % 100 < 30);

	// Different weapon behaviors
	switch (weapon)
	{
		case PEASHOOTER:
			velocityX = dirX * 12.0f;
			velocityY = dirY * 12.0f;
			projectileColor = parryable ? Color(255, 192, 203) : Color::Yellow;
			size = 5.0f;
			maxLifetime = 8.0f;
			break;

		case SPREAD:
			velocityX = dirX * 10.0f;
			velocityY = dirY * 10.0f;
			projectileColor = parryable ? Color(255, 192, 203) : Color(255, 165, 0);
			size = 4.0f;
			maxLifetime = 6.0f;
			break;

		case CHASER:
			velocityX = dirX * 8.0f;
			velocityY = dirY * 8.0f;
			projectileColor = parryable ? Color(255, 192, 203) : Color::Red;
			size = 6.0f;
			maxLifetime = 15.0f;
			break;

		case LOBBER:
			velocityX = dirX * 6.0f;
			velocityY = dirY * 6.0f - 5.0f;
			projectileColor = parryable ? Color(255, 192, 203) : Color::Magenta;
			size = 7.0f;
			maxLifetime = 10.0f;
			break;

		case CHARGE:
			velocityX = dirX * 15.0f;
			velocityY = dirY * 15.0f;
			projectileColor = parryable ? Color(255, 192, 203) : Color::Cyan;
			size = 8.0f;
			maxLifetime = 5.0f;
			damage = dmg * 2;
			break;

		case ROUNDABOUT:
			velocityX = dirX * 10.0f;
			velocityY = dirY * 10.0f;
			projectileColor = parryable ? Color(255, 192, 203) : Color::Green;
			size = 5.0f;
			maxLifetime = 12.0f;
			break;
	}
}

void Projectile::update()
{
	if (!active) return;

	lifetime += 0.016f;
	if (lifetime > maxLifetime)
	{
		active = false;
		return;
	}

	sprite.move(velocityX, velocityY);

	if (weaponType != CHASER && weaponType != ROUNDABOUT)
	{
		velocityY += GRAVITY * 0.3f;
	}
	else if (weaponType == CHASER)
	{
		velocityY += GRAVITY * 0.1f;
	}

	if (sprite.getPosition().x < 0 || sprite.getPosition().x > WORLD_WIDTH ||
		sprite.getPosition().y < 0 || sprite.getPosition().y > SCREEN_Y)
	{
		active = false;
	}
}

void Projectile::draw(RenderWindow& window)
{
	if (!active) return;

	Vector2f pos = sprite.getPosition();

	switch (weaponType)
	{
		case PEASHOOTER:
		{
			CircleShape pea(size);
			pea.setPosition(pos);
			pea.setFillColor(Color::Yellow);
			window.draw(pea);

			CircleShape peaOutline(size);
			peaOutline.setPosition(pos);
			peaOutline.setFillColor(Color::Transparent);
			peaOutline.setOutlineColor(Color(200, 200, 0));
			peaOutline.setOutlineThickness(1);
			window.draw(peaOutline);
			break;
		}

		case SPREAD:
		{
			for (int i = 0; i < 3; i++)
			{
				CircleShape pellet(size - 1);
				pellet.setPosition(pos + Vector2f(i * 2, 0));
				pellet.setFillColor(Color(255, 165, 0));
				window.draw(pellet);
			}
			break;
		}

		case CHASER:
		{
			// Red homing missile shape (pointed cone)
			ConvexShape missile(4);
			missile.setPoint(0, Vector2f(0, -size));      // Top point
			missile.setPoint(1, Vector2f(size, 0));       // Right
			missile.setPoint(2, Vector2f(0, size));       // Bottom
			missile.setPoint(3, Vector2f(-size, 0));      // Left
			missile.setPosition(pos + Vector2f(size, size));
			missile.setFillColor(Color::Red);
			window.draw(missile);

			// Fire trail effect
			CircleShape trail(size - 2);
			trail.setPosition(pos + Vector2f(2, 2));
			trail.setFillColor(Color(255, 100, 0, 100));
			window.draw(trail);
			break;
		}

		case LOBBER:
		{
			// Magenta bomb/shell shape (large circle with details)
			CircleShape bomb(size);
			bomb.setPosition(pos);
			bomb.setFillColor(Color::Magenta);
			window.draw(bomb);

			// Bomb shine
			CircleShape shine(size / 2);
			shine.setPosition(pos + Vector2f(size / 3, size / 3));
			shine.setFillColor(Color(255, 200, 255, 150));
			window.draw(shine);

			// Fuse effect
			RectangleShape fuse(Vector2f(1, size + 3));
			fuse.setPosition(pos + Vector2f(size - 1, -3));
			fuse.setFillColor(Color::Black);
			window.draw(fuse);
			break;
		}

		case CHARGE:
		{
			// Cyan charged energy bolt with glow
			CircleShape bolt(size);
			bolt.setPosition(pos);
			bolt.setFillColor(Color::Cyan);
			window.draw(bolt);

			// Inner bright core
			CircleShape core(size - 1);
			core.setPosition(pos + Vector2f(1, 1));
			core.setFillColor(Color(100, 255, 255));
			window.draw(core);

			// Outer glow effect
			CircleShape glow(size + 3);
			glow.setPosition(pos - Vector2f(3, 3));
			glow.setFillColor(Color::Transparent);
			glow.setOutlineColor(Color(0, 255, 255, 150));
			glow.setOutlineThickness(2);
			window.draw(glow);

			// Electric effect (small spikes)
			for (int i = 0; i < 4; i++)
			{
				float angle = (i * 3.14159f) / 2;
				float spikeX = pos.x + size * cos(angle);
				float spikeY = pos.y + size * sin(angle);
				
				CircleShape spike(2);
				spike.setPosition(spikeX - 2, spikeY - 2);
				spike.setFillColor(Color(255, 255, 100));
				window.draw(spike);
			}
			break;
		}

		case ROUNDABOUT:
		{
			// Green spinning projectile shape
			ConvexShape star(6);
			for (int i = 0; i < 6; i++)
			{
				float angle = (i * 3.14159f) / 3;
				float x = size * cos(angle);
				float y = size * sin(angle);
				star.setPoint(i, Vector2f(x, y));
			}
			star.setPosition(pos + Vector2f(size, size));
			star.setFillColor(Color::Green);
			window.draw(star);

			// Center circle
			CircleShape center(size / 2);
			center.setPosition(pos + Vector2f(size / 2, size / 2));
			center.setFillColor(Color(0, 200, 0));
			window.draw(center);
			break;
		}
	}
}

bool Projectile::isActive() const { return active; }
void Projectile::setInactive() { active = false; }
FloatRect Projectile::getBounds() const 
{ 
	Vector2f pos = sprite.getPosition();
	return FloatRect(pos.x - size, pos.y - size, size * 2, size * 2);
}
int Projectile::getDamage() const { return damage; }
WeaponType Projectile::getWeaponType() const { return weaponType; }
bool Projectile::getParryable() const { return parryable; }
