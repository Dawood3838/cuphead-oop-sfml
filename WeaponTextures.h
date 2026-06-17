#ifndef WEAPON_TEXTURES_H
#define WEAPON_TEXTURES_H

#include <SFML/Graphics.hpp>

using namespace sf;

class WeaponTextureManager
{
private:
	Texture peashooterTexture;
	bool peashooterLoaded;

public:
	WeaponTextureManager() : peashooterLoaded(false)
	{
		loadTextures();
	}

	void loadTextures()
	{
		if (!peashooterTexture.loadFromFile("Data/peashooter.png.png"))
		{
			peashooterLoaded = false;
		}
		else
		{
			peashooterLoaded = true;
		}
	}

	bool isPeashooterLoaded() const { return peashooterLoaded; }
	Texture& getPeashooterTexture() { return peashooterTexture; }
};

#endif
