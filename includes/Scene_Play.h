#pragma once

#include "Scene.h"
#include <map>
#include <memory>

#include "EntityManager.h"

class Scene_Play : public Scene
{
	struct PlayerConfig
	{
		float X, Y, CX, CY, SPEED, MAXSPEED, JUMP, GRAVITY;
		std::string WEAPON;
	};

protected:

	std::shared_ptr<Entity>	m_player;
	std::string				m_levelPath;
	PlayerConfig			m_playerConfig;
	bool					m_drawTextures = true;
	bool					m_drawCollision = false;
	bool					m_drawGrid = false;
	const Vec2				m_gridSize = { 64, 64 };
	sf::Text				m_gridText;
	float					m_maxJumpHeight = m_gridSize.y * 5;;

	sf::Clock deltaClock;

	void init(const std::string& levelPath);

	void loadLevel(const std::string& fileName);

	void onEnd() override;
	void update() override;
	void doAction(const Action& action) override;

	void spawnPlayer();
	void spawnBullet(std::shared_ptr<Entity> entity);

	void sMovement(float deltaTime);
	void sLifespan();
	void sCollision();
	void sAnimation();
	void sRender() override;
	void sDoAction(const Action& action) override;

	Vec2 gridToMidPixel(float gridX, float gridY, std::shared_ptr<Entity> entity);

public:
	Scene_Play();
	Scene_Play(GameEngine* gameEngine, const std::string& levelPath);
	
};