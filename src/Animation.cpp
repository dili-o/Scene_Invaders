#include "Animation.h"
#include <cmath>
#include <iostream>

Animation::Animation()
{
}

Animation::Animation(const std::string& name, const sf::Texture& t)
	:Animation(name, t, 1, 0)
{
}

Animation::Animation(const std::string& name, const sf::Texture& t, size_t frameCount, size_t speed)
	:m_name(name), m_sprite(t), m_frameCount(frameCount), m_currentFrame(0), m_speed(speed)
{
	m_size = Vec2((float)t.getSize().x / frameCount, (float)t.getSize().y);
	m_sprite.setOrigin(m_size.x / 2.f, m_size.y / 2.f);
	m_sprite.setTextureRect(sf::IntRect(std::floor(m_currentFrame) * m_size.x, 0, m_size.x, m_size.y));
}


// Update the animation to show the next frame, depending on its speed
// Animation loops once it reaches the end
void Animation::update()
{
	// add the speed variable to the current frame
	m_currentFrame++;

	// TODO: 1) calculate the correct frame of animation to play based on the currentframe and speed
	//		 2) set the texture rectangle properly
	//DONE
	int animationFrame = (m_currentFrame / (m_speed + 1)) % m_frameCount;
	m_sprite.setTextureRect(sf::IntRect(std::floor(animationFrame) * m_size.x, 0, m_size.x, m_size.y));
}

bool Animation::hasEnded() const
{
	// TODO
	// DONE
	return (m_currentFrame == m_frameCount);
}

const std::string& Animation::getName() const
{
	return m_name;
}

const Vec2& Animation::getSize() const
{
	return m_size;
}

sf::Sprite& Animation::getSprite()
{
	return m_sprite;
}
