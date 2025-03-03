#include "Scene.h"
#include "GameEngine.h"

void Scene::setPaused(bool paused)
{
	m_paused = paused;
}

Scene::Scene()
{
}

Scene::Scene(GameEngine* gameEngine)
	:m_game(gameEngine)
{
}

void Scene::doAction(const Action& action)
{
	sDoAction(action);
}

void Scene::registerAction(int inputKey, const std::string& actionName)
{
	m_actionMap[inputKey] = actionName;
}

size_t Scene::width() const
{
	return m_game->window().getDefaultView().getSize().x;
}

size_t Scene::height() const
{
	return m_game->window().getDefaultView().getSize().y;
}

size_t Scene::currentFrame() const
{
	return m_currentFrame;
}

bool Scene::hasEnded() const
{
	return m_hasEnded;
}

const ActionMap& Scene::getActionMap() const
{
	return m_actionMap;
}

void Scene::drawLine(const Vec2& p1, const Vec2& p2)
{
	sf::VertexArray line(sf::Lines, 2);

	line[0].position = sf::Vector2f(p1.x, p1.y);
	line[0].color = sf::Color::White;
	line[1].position = sf::Vector2f(p2.x, p2.y);
	line[1].color = sf::Color::White;

	m_game->window().draw(line);
}
