#pragma once

#include "Vec2.h"

#include <vector>
#include <SFML/Graphics.hpp>

class Animation
{
private:
	sf::Sprite	m_sprite;
	size_t		m_frameCount	= 1;		// Total number of frames of animation
	size_t		m_currentFrame	= 0;		// The current frame of the animation
	size_t		m_speed			= 0;		// The speed to play the animation
	Vec2		m_size			= { 1,1 };	// The size of the animation frame
	std::string	m_name			= "none";

public:
	Animation();
	Animation(const std::string& name, const sf::Texture& t);
	Animation(const std::string& name, const sf::Texture& t, size_t frameCount, size_t speed);

	void update();
	bool hasEnded() const;
	const std::string& getName() const;
	const Vec2& getSize() const;
	sf::Sprite& getSprite();
};