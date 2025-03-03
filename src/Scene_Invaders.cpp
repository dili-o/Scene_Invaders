#include "Scene_Invaders.h"
#include "Scene_Menu.h"
#include "Physics.h"
#include "Assets.h"
#include "GameEngine.h"
#include "Components.h"
#include "Action.h"
#include <iostream>


Scene_Invaders::Scene_Invaders()
{
}

Scene_Invaders::Scene_Invaders(GameEngine* gameEngine)
	:Scene(gameEngine)
{
	init();
}

void Scene_Invaders::init()
{
	registerAction(sf::Keyboard::P, "PAUSE");
	registerAction(sf::Keyboard::Escape, "QUIT");
	registerAction(sf::Keyboard::Enter, "START");
	registerAction(sf::Keyboard::T, "TOGGLE_TEXTURE");
	registerAction(sf::Keyboard::C, "TOGGLE_COLLISION");
	registerAction(sf::Keyboard::G, "TOGGLE_GRID");
	registerAction(sf::Keyboard::Left, "LEFT");
	registerAction(sf::Keyboard::A, "LEFT");
	registerAction(sf::Keyboard::Right, "RIGHT");
	registerAction(sf::Keyboard::D, "RIGHT");
	registerAction(sf::Keyboard::Space, "SHOOT");
	// TODO: Add the movement actions and shoot actions

	// TODO: Set up the UI
	scoreLabel = sf::Text("Score<1>", m_game->assets().getFont("SpaceInvaders"), 8);
	scoreLabel.setFillColor(sf::Color::White);
	scoreLabel.setPosition(10, 0);

	scoreText = sf::Text("0000", m_game->assets().getFont("SpaceInvaders"), 8);
	scoreText.setFillColor(sf::Color::White);
	scoreText.setPosition(15, 18);

	hiScoreLabel = sf::Text("HI-score", m_game->assets().getFont("SpaceInvaders"), 8);
	hiScoreLabel.setFillColor(sf::Color::White);
	hiScoreLabel.setPosition(89, 0);

	hiScoreText = sf::Text("0000", m_game->assets().getFont("SpaceInvaders"), 8);
	hiScoreText.setFillColor(sf::Color::White);
	hiScoreText.setPosition(94, 18);

	playerLivesText = sf::Text("3", m_game->assets().getFont("SpaceInvaders"), 8);
	playerLivesText.setFillColor(sf::Color::White);
	playerLivesText.setPosition(5, 248);
	// Remove anti aliasing form text
	auto& texture = const_cast<sf::Texture&>(m_game->assets().getFont("SpaceInvaders").getTexture(8));
	texture.setSmooth(false);
	// TODO: Spawn the shelters
	const int shelterPosX[4] = { 31, 84, 143, 195 };
	for (int i = 0; i < 4; i++)
	{
		auto shelter = m_entityManager.addEntity("shelter");
		shelter->addComponent<CAnimation>(m_game->assets().getAnimation("Shelter"), true);
		shelter->addComponent<CTransform>(Vec2(shelterPosX[i], 203));
		shelter->addComponent<CBoundingBox>(Vec2(32, 24));
	}
	spawnPlayer();
	
}

void Scene_Invaders::onEnd()
{
	m_game->changeScene("MENU", std::make_shared<Scene_Menu>(m_game));
}

void Scene_Invaders::update()
{
	m_entityManager.update();
	float deltaTime = deltaClock.restart().asSeconds();
	if (!m_paused)
	{
		sMovement(deltaTime);
		sLifespan();
		sCollision();
		sAnimation();
	}
	sRender();
}

void Scene_Invaders::doAction(const Action& action)
{
	sDoAction(action);
}

void Scene_Invaders::spawnPlayer()
{
	m_player = m_entityManager.addEntity("player");
	m_player->addComponent<CAnimation>(m_game->assets().getAnimation("Player"), true);
	m_player->addComponent<CTransform>(Vec2(15, 236));
	m_player->addComponent<CBoundingBox>(Vec2(16, 8));
	m_player->addComponent<CInput>();
}

void Scene_Invaders::spawnEnemies()
{
	Vec2 startPos(10, 40);
	float colGap = 16;
	float rowGap = 15;
	float rowCount = 5;
	float colCount = 11;
	for (int row = 0; row < rowCount; row++)
	{
		
		int alienType = 0;
		if (row == 0)
			alienType = 3;
		else if (row < 3)
			alienType = 2;
		else
			alienType = 1;
		for (int col = 0; col < colCount; col++)
		{
			std::string alienTex = "Alien" + std::to_string(alienType);
			std::shared_ptr<Entity> enemy = m_entityManager.addEntity("enemy");
			enemy->addComponent<CAnimation>(m_game->assets().getAnimation(alienTex), true);
			enemy->addComponent<CTransform>(Vec2(startPos.x + (col * colGap), startPos.y + (row * rowGap)));
			enemy->addComponent<CBoundingBox>(Vec2(12, 8));
		}
	}
	gameStart = true;
}

void Scene_Invaders::spawnBullet(std::shared_ptr<Entity> entity)
{
	if (entity->tag() == "player")
	{
		std::shared_ptr<Entity> bullet = m_entityManager.addEntity("playerBullet");
		bullet->addComponent<CAnimation>(m_game->assets().getAnimation("PlayerBullet"), true);
		Vec2 pos = entity->getComponent<CTransform>().pos;
		pos.y -= 4;
		bullet->addComponent<CTransform>(pos);
		bullet->getComponent<CTransform>().velocity.y = -200;
		bullet->addComponent<CBoundingBox>(Vec2(3, 4));
	}
	else if (entity->tag() == "enemy")
	{
		std::shared_ptr<Entity> bullet = m_entityManager.addEntity("enemyBullet");
		std::string bulletType = "AlienLaser";
		bulletType.push_back(entity->getComponent<CAnimation>().animation.getName()[5]);
		bullet->addComponent<CAnimation>(m_game->assets().getAnimation(bulletType), true);
		Vec2 pos = entity->getComponent<CTransform>().pos;
		pos.y += 4;
		bullet->addComponent<CTransform>(pos);
		bullet->getComponent<CTransform>().velocity.y = 200;
		bullet->addComponent<CBoundingBox>(Vec2(3, 4));
	}
}

void Scene_Invaders::sMovement(float deltaTime){
	// Set the previous position
	for (auto e : m_entityManager.getEntities())
	{
		e->getComponent<CTransform>().prevPos = e->getComponent<CTransform>().pos;
	}

	// Alien Movement
	if (gameTick < 0)
	{
		for (auto e : m_entityManager.getEntities("enemy"))
		{
			if (descend)
			{
				e->getComponent<CTransform>().pos.y += 2;
				if (e->getComponent<CTransform>().pos.y > height() - 10)
				{
					resetGame();
				}
			}
			else
			{
				e->getComponent<CTransform>().pos.x += 2 * alienDirectionX;
				e->getComponent<CAnimation>().animation.update();
				if (e->getComponent<CTransform>().pos.x < 8 || e->getComponent<CTransform>().pos.x >(width() - 8))
				{
					boundReached = true;
				}
			}
		}
		descend = false;
		gameTick = 1.f / alienSpeed;
	}
	else
	{
		gameTick -= deltaTime;
	}
	if (boundReached)
	{
		alienDirectionX *= -1;
		boundReached = false;
		descend = true;
	}
	// Player Movement
	m_player->getComponent<CTransform>().velocity.x = 0;
	if (m_player->getComponent<CInput>().left)
	{
		m_player->getComponent<CTransform>().velocity.x = -1;
	}
	else if (m_player->getComponent<CInput>().right)
	{
		m_player->getComponent<CTransform>().velocity.x = 1;
	}
	// Apply velocity on the player
	m_player->getComponent<CTransform>().pos.x += m_player->getComponent<CTransform>().velocity.x;
	const char* movingEntities[3] = { "player", "playerBullet", "enemyBullet" };
	for (int i = 0; i < 3; i++)
	{
		for (auto e : m_entityManager.getEntities(movingEntities[i]))
		{
			e->getComponent<CTransform>().pos.y += e->getComponent<CTransform>().velocity.y * deltaTime;
		}
	}
	
}

void Scene_Invaders::sLifespan()
{
}

void Scene_Invaders::sCollision()
{
	// Border collision with the player
	if (m_player->getComponent<CTransform>().pos.x < 8 || m_player->getComponent<CTransform>().pos.x > 216)
	{
		m_player->getComponent<CTransform>().pos.x = m_player->getComponent<CTransform>().prevPos.x;
	}
	// Player bullet Collisions
	for (auto b : m_entityManager.getEntities("playerBullet"))
	{
		// Player bullet collision with border
		if (b->getComponent<CTransform>().pos.y < 0 || b->getComponent<CTransform>().pos.y > height())
		{
			b->destroy();
			m_player->getComponent<CInput>().canShoot = true;
		}
		// Player bullet collision with aliens
		for (auto alien : m_entityManager.getEntities("enemy"))
		{
			if (!b->isActive()) { continue; }
			Vec2 bOverlap = Physics::GetOverlap(alien, b);
			if (bOverlap.x > 0.f && bOverlap.y > 0.f)
			{
				b->destroy();
				m_player->getComponent<CInput>().canShoot = true;
				auto explosion = m_entityManager.addEntity("death");
				explosion->addComponent<CAnimation>(m_game->assets().getAnimation("Death"), false);
				explosion->addComponent<CTransform>(alien->getComponent<CTransform>().pos);
				explosion->getComponent<CTransform>().scale.y = 0.46153846;
				explosion->getComponent<CTransform>().scale.x = 0.5f;
				alien->destroy();
				alienKillCount++;
				if ((alienKillCount % speedUpThreshold) == 0)
				{
					alienSpeed += speedUpAmount;
				}
				if (alien->getComponent<CAnimation>().animation.getName() == "Alien1")
					score += 10;
				else if (alien->getComponent<CAnimation>().animation.getName() == "Alien2")
					score += 20;
				else if (alien->getComponent<CAnimation>().animation.getName() == "Alien3")
					score += 30;
				scoreText.setString(std::to_string(score));

				if (alienKillCount == 55)
				{
					resetGame();
				}
			}
		}
		// Player bullet collision with the shelter
		for (auto shelter : m_entityManager.getEntities("shelter"))
		{
			if (!b->isActive()) { continue; }
			Vec2 bOverlap = Physics::GetOverlap(shelter, b);
			if (bOverlap.x > 0.f && bOverlap.y > 0.f)
			{
				b->destroy();
				m_player->getComponent<CInput>().canShoot = true;
				auto explosion = m_entityManager.addEntity("explosion");
				explosion->addComponent<CAnimation>(m_game->assets().getAnimation("Explosion"), false);
				explosion->addComponent<CTransform>(b->getComponent<CTransform>().pos);
			}
		}
	}
	// Enemy bullet collisions
	for (auto b : m_entityManager.getEntities("enemyBullet"))
	{
		// Enemy bullet collisions with the border
		if (b->getComponent<CTransform>().pos.y < 0 || b->getComponent<CTransform>().pos.y > height() - 14)
		{
			b->destroy();
			alienCanShoot = true;
			auto explosion = m_entityManager.addEntity("explosion");
			explosion->addComponent<CAnimation>(m_game->assets().getAnimation("Explosion"), false);
			explosion->addComponent<CTransform>(b->getComponent<CTransform>().pos);
		}
		// Enemy Bullet collision with shelter
		for (auto shelter : m_entityManager.getEntities("shelter"))
		{
			if (!b->isActive()) { continue; }
			Vec2 bOverlap = Physics::GetOverlap(shelter, b);
			if (bOverlap.x > 0.f && bOverlap.y > 0.f)
			{
				b->destroy();
				alienCanShoot = true;
				auto explosion = m_entityManager.addEntity("explosion");
				explosion->addComponent<CAnimation>(m_game->assets().getAnimation("Explosion"), false);
				explosion->addComponent<CTransform>(b->getComponent<CTransform>().pos);
			}
		}
		// Enemy Bullet collision with player
		if (!b->isActive()) { continue; }
		Vec2 bOverlap = Physics::GetOverlap(m_player, b);
		if (bOverlap.x > 0.f && bOverlap.y > 0.f)
		{
			b->destroy();
			alienCanShoot = true;
			auto explosion = m_entityManager.addEntity("explosion");
			explosion->addComponent<CAnimation>(m_game->assets().getAnimation("Explosion"), false);
			explosion->addComponent<CTransform>(b->getComponent<CTransform>().pos);

			auto death = m_entityManager.addEntity("death");
			death->addComponent<CAnimation>(m_game->assets().getAnimation("Death"), false);
			death->addComponent<CTransform>(m_player->getComponent<CTransform>().pos);
			death->getComponent<CTransform>().scale.y = 0.46153846;
			death->getComponent<CTransform>().scale.x = 0.5f;
			m_player->destroy();
			// On player death
			if (lives == 0)
			{
				resetGame();
			}
			else
			{
				lives--;
			}
			playerLivesText.setString(std::to_string(lives));
			spawnPlayer();
			
		}
	}
	// Raycast with aliens
	Vec2 playerPos = m_player->getComponent<CTransform>().pos;
	std::shared_ptr<Entity> targetAlien;
	for (auto e : m_entityManager.getEntities("enemy"))
	{
		if (Physics::EntityIntersect(playerPos, Vec2(playerPos.x, 0), e))
		{
			if (targetAlien && (targetAlien->getComponent<CTransform>().pos.y <= e->getComponent<CTransform>().pos.y))
			{
				targetAlien = e;
			}
			else
			{
				targetAlien = e;
			}
		}
	}
	// Checks if raycast collides with a shelter
	for (auto shelter : m_entityManager.getEntities("shelter"))
	{
		if (Physics::EntityIntersect(playerPos, Vec2(playerPos.x, 0), shelter))
		{
			targetAlien = nullptr;
		}
	}

	if (targetAlien && alienCanShoot)
	{
		spawnBullet(targetAlien);
		alienCanShoot = false;
	}
}

void Scene_Invaders::sAnimation()
{
	for (auto e : m_entityManager.getEntities())
	{
		if (e->hasComponent<CAnimation>())
		{
			if (!e->getComponent<CAnimation>().repeat)
			{
				if (e->getComponent<CAnimation>().animation.hasEnded())
				{
					e->destroy();
				}
				e->getComponent<CAnimation>().animation.update();
			}
			if (e->tag() == "enemyBullet")
			{
				e->getComponent<CAnimation>().animation.update();
			}
		}
	}
}

void Scene_Invaders::sRender()
{
	m_game->window().clear(sf::Color(0, 0, 0));

	
	if (m_drawTextures)
	{
		for (auto e : m_entityManager.getEntities())
		{
			if (e->hasComponent<CAnimation>())
			{
				auto& transform = e->getComponent<CTransform>();
				auto& animaiton = e->getComponent<CAnimation>().animation;
				animaiton.getSprite().setRotation(transform.angle);
				animaiton.getSprite().setPosition(transform.pos.x, transform.pos.y);
				animaiton.getSprite().setScale(transform.scale.x, transform.scale.y);
				m_game->window().draw(animaiton.getSprite());
				
			}
		}
	}
	// draw all Enitiy collison bounding boxes with a rectangle shape
	if (m_drawCollision)
	{
		for (auto e : m_entityManager.getEntities())
		{
			if (e->hasComponent<CBoundingBox>())
			{
				auto& box = e->getComponent<CBoundingBox>();
				auto& tranform = e->getComponent<CTransform>();
				sf::RectangleShape rect;
				rect.setPosition(tranform.pos.x, tranform.pos.y);
				rect.setSize(sf::Vector2f(box.size.x - 1, box.size.y - 1));
				rect.setOrigin(sf::Vector2f(box.halfSize.x, box.halfSize.y));
				rect.setFillColor(sf::Color(135, 206, 250, 100));
				rect.setOutlineColor(sf::Color(135, 206, 250, 100));
				rect.setOutlineThickness(1.f);
				m_game->window().draw(rect);
			}
		}
	}

	sf::RectangleShape line(sf::Vector2f(width(), 1));
	line.setFillColor(sf::Color::Green);
	line.setPosition(sf::Vector2f(0, height() - 10));

	m_game->window().draw(line);
	m_game->window().draw(scoreLabel);
	m_game->window().draw(scoreText);
	m_game->window().draw(hiScoreLabel);
	m_game->window().draw(hiScoreText);
	m_game->window().draw(playerLivesText);
	m_game->window().display();
}

void Scene_Invaders::sDoAction(const Action& action)
{
	if (action.type() == "START")
	{
		if (action.name() == "TOGGLE_TEXTURE") { m_drawTextures = !m_drawTextures; }
		else if (action.name() == "TOGGLE_COLLISION") { m_drawCollision = !m_drawCollision; }
		else if (action.name() == "PAUSE") { setPaused(!m_paused); }
		else if (action.name() == "QUIT") { onEnd(); }
		else if (action.name() == "LEFT") { m_player->getComponent<CInput>().left = true; }
		else if (action.name() == "RIGHT") { m_player->getComponent<CInput>().right = true; }
		else if (action.name() == "SHOOT")
		{
			if (m_player->getComponent<CInput>().canShoot)
			{
				spawnBullet(m_player);
				m_player->getComponent<CInput>().canShoot = false;
			}
		}
		else if (action.name() == "START")
		{
			if (!gameStart)
			{
				spawnEnemies();
			}
		}
	}
	else if (action.type() == "END")
	{
	if (action.name() == "LEFT") { m_player->getComponent<CInput>().left = false; }
	else if (action.name() == "RIGHT") { m_player->getComponent<CInput>().right = false; }
	}
}

void Scene_Invaders::resetGame()
{
	lives = 3;
	score = 0;
	scoreText.setString("0000");
	// Reset aliens
	boundReached = false;
	descend = false;
	gameStart = false;
	alienDirectionX = -1.f;
	gameTick = 1.f;
	speedUpThreshold = 5;
	speedUpAmount = 3;
	alienKillCount = 0;
	alienSpeed = 1.f;
	alienCanShoot = true;

	for (auto e : m_entityManager.getEntities("enemy"))
	{
		e->destroy();
	}
}
