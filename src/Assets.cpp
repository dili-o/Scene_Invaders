#include "Assets.h"
#include <iostream>
#include <fstream>
#include <sstream>

void Assets::addTexture(const std::string& name, const std::string& path)
{
	sf::Texture texture;
	if (!texture.loadFromFile(path))
	{
		std::cout << "Error loading file at " << path << '\n';
		return;
	}
	m_textures[name] = texture;
	std::cout << "Successfully added Texture: " << name << '\n';
}

void Assets::addAnimation(const std::string& name, Animation& animation)
{
	m_animations[name] = animation;
}

void Assets::addSound(const std::string& name, const std::string& path)
{
	sf::SoundBuffer buffer;
	if (!buffer.loadFromFile(path))
	{
		std::cout << "Error loading file at " << path << "\n";
		return;
	}
	sf::Sound sound;
	sound.setBuffer(buffer);
	m_sounds[name] = sound;
	std::cout << "Successfully added Sound: " << name << '\n';
}

void Assets::addFont(const std::string& name, const std::string& path)
{
	sf::Font font;
	if (!font.loadFromFile(path))
	{
		std::cout << "Error loading file at " << path << "\n";
		return;
	}
	m_fonts[name] = font;
	std::cout << "Successfully added Font: " << name << '\n';
}

const sf::Texture& Assets::getTexture(const std::string& name) const
{
	auto texture = m_textures.find(name);
	if (texture == m_textures.end())
	{
		std::cout << "Texture (" << name << ") does not exist!\n";
	}
	return texture->second;
}

const Animation& Assets::getAnimation(const std::string& name) const
{
	auto anim = m_animations.find(name);
	if (anim == m_animations.end())
	{
		std::cout << "Animation (" << name << ") does not exist!\n";
	}
	return anim->second;
}

const sf::Sound& Assets::getSound(const std::string& name) const
{
	auto sound = m_sounds.find(name);
	if (sound == m_sounds.end())
	{
		std::cout << "Sound (" << name << ") does not exist!\n";
	}
	return sound->second;
}

const sf::Font& Assets::getFont(const std::string& name) const
{
	auto font = m_fonts.find(name);
	if (font == m_fonts.end())
	{
		std::cout << "Font (" << name << ") does not exist!\n";
	}
	return font->second;
}

void Assets::loadFromFile(const std::string& path)
{
	
	std::ifstream file(path);

	if (!file.is_open()) {
		std::cerr << "Unable to open file!" << std::endl;
		return;
	}

	std::string line;

	while (std::getline(file, line)) {
		std::istringstream iss(line);
		std::vector<std::string> tokens;
		std::string token;

		while (iss >> token) {
			tokens.push_back(token);
		}
		
		if (tokens[0] == "Texture")
		{
			addTexture(tokens[1], tokens[2]);
		}
		else if (tokens[0] == "Animation")
		{
			Animation a(tokens[1], getTexture(tokens[2]), std::stoul(tokens[3]), std::stoull(tokens[4]));
			addAnimation(a.getName(), a);
		}
		else if (tokens[0] == "Font")
		{
			addFont(tokens[1], tokens[2]);
		}
	}
}
