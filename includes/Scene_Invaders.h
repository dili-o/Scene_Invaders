#pragma once

#include "Scene.h"
#include <map>
#include <memory>

#include "EntityManager.h"

class Scene_Invaders : public Scene
{
protected:

	std::shared_ptr<Entity>	m_player;
	bool					m_drawTextures = true;
	bool					m_drawCollision = false;

	sf::Clock deltaClock;

	void init();

	void onEnd() override;
	void update() override;
	void doAction(const Action& action) override;

	void spawnPlayer();
	void spawnEnemies();
	void spawnBullet(std::shared_ptr<Entity> entity);

	// Alien related stuff
	bool boundReached = false;
	bool descend = false;
	bool gameStart = false;
	float alienDirectionX = -1.f;
	float gameTick = 1.f;
	int speedUpThreshold = 5;
	float speedUpAmount = 3;
	int alienKillCount = 0;
	float alienSpeed = 1.f;
	bool alienCanShoot = true;

	// UI stuff
	sf::Text scoreLabel;
	sf::Text scoreText;
	sf::Text hiScoreLabel;
	sf::Text hiScoreText;
	sf::Text playerLivesText;

	// Player stuff
	int score = 0;
	int lives = 3;

	void sMovement(float deltaTime);
	void sLifespan();
	void sCollision();
	void sAnimation();
	void sRender() override;
	void sDoAction(const Action& action) override;
	void resetGame();

public:

	Scene_Invaders();
	Scene_Invaders(GameEngine* gameEngine);
};

