#pragma once

#include "Scene.h"
#include <map>
#include <memory>
#include <deque>

#include "EntityManager.h"

class Scene_Menu : public Scene
{

protected:

	std::string					m_title;
	sf::Text					m_menuText;

	void init();
	void update();
	void onEnd();
	void sDoAction(const Action& action);
	
public:
	
	Scene_Menu(GameEngine* gameEngine = nullptr);
	void sRender() override;
};
