#include "Scene_Menu.h"
#include "Scene_Play.h"
#include "GameEngine.h"
#include "Scene_Invaders.h"
#include <iostream>

Scene_Menu::Scene_Menu(GameEngine* gameEngine)
{
	m_game = gameEngine;
	init();
}

void Scene_Menu::init()
{
	registerAction(sf::Keyboard::Escape, "QUIT");
	registerAction(sf::Keyboard::Space, "PLAY");

	m_title = "Space Invaders";

	m_menuText = sf::Text(m_title, m_game->assets().getFont("SpaceInvaders"), 21);
	m_menuText.setFillColor(sf::Color::Green);
	m_menuText.setPosition(0, 0);
	// Remove anti aliasing form text
	auto& texture = const_cast<sf::Texture&>(m_game->assets().getFont("SpaceInvaders").getTexture(21));
	texture.setSmooth(false);
}

void Scene_Menu::update()
{
	sRender();
}

void Scene_Menu::onEnd()
{
	m_game->quit();
}

void Scene_Menu::sDoAction(const Action& action)
{
	if (action.type() == "START")
	{
		if (action.name() == "PLAY")
		{
			m_game->changeScene("PLAY", std::make_shared<Scene_Invaders>(m_game));
		}
		else if (action.name() == "QUIT") { onEnd(); }
	}
	else if (action.type() == "END")
	{
	}
}



void Scene_Menu::sRender()
{
	m_game->window().clear(sf::Color::Black);

	m_game->window().draw(m_menuText);

	m_game->window().display();
}
