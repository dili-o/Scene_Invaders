#include "Scene_Play.h"
#include "Scene_Menu.h"
#include "Physics.h"
#include "Assets.h"
#include "GameEngine.h"
#include "Components.h"
#include "Action.h"

#include <iostream>
#include <fstream>
#include <sstream>
 


Scene_Play::Scene_Play()
{
	
}

Scene_Play::Scene_Play(GameEngine* gameEngine, const std::string& levelPath)
	:Scene(gameEngine), m_levelPath(levelPath)
{
	init(m_levelPath);
}

void Scene_Play::init(const std::string& levelPath)
{
	registerAction(sf::Keyboard::P,		"PAUSE");
	registerAction(sf::Keyboard::Escape,"QUIT");
	registerAction(sf::Keyboard::T,		"TOGGLE_TEXTURE");
	registerAction(sf::Keyboard::C,		"TOGGLE_COLLISION");
	registerAction(sf::Keyboard::G,		"TOGGLE_GRID");
	registerAction(sf::Keyboard::W,		"UP");
	registerAction(sf::Keyboard::A,		"LEFT");
	registerAction(sf::Keyboard::D,		"RIGHT");
	registerAction(sf::Keyboard::S,		"DOWN");
	registerAction(sf::Keyboard::Space,	"SHOOT");

	m_gridText.setCharacterSize(10);
	m_gridText.setFont(m_game->assets().getFont("Mario"));

	loadLevel(levelPath);
}

Vec2 Scene_Play::gridToMidPixel(float gridX, float gridY, std::shared_ptr<Entity> entity)
{
	Vec2 texureSize = entity->getComponent<CAnimation>().animation.getSize();

	Vec2 pos(gridX * m_gridSize.x, height() - (gridY * m_gridSize.y));
	pos.x += texureSize.x / 2;
	pos.y -= texureSize.y / 2;
	return pos;
}

void Scene_Play::loadLevel(const std::string& fileName)
{
	// reset the entity manager everytime we load a level
	m_entityManager = EntityManager();

	std::ifstream file(fileName);

	if (!file.is_open()) {
		std::cerr << "Unable to open Level!" << std::endl;
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

		if (tokens[0] == "Dec")
		{
			auto dec = m_entityManager.addEntity("dec");
			dec->addComponent<CAnimation>(m_game->assets().getAnimation(tokens[1]), true);
			dec->addComponent<CTransform>(gridToMidPixel(std::stof(tokens[2]), std::stof(tokens[3]), dec));
			
		}
		else if (tokens[0] == "Tile")
		{
			auto dec = m_entityManager.addEntity("tile");
			dec->addComponent<CAnimation>(m_game->assets().getAnimation(tokens[1]), true);
			dec->addComponent<CTransform>(gridToMidPixel(std::stof(tokens[2]), std::stof(tokens[3]), dec));
			dec->addComponent<CBoundingBox>(Vec2(dec->getComponent<CAnimation>().animation.getSize().x, dec->getComponent<CAnimation>().animation.getSize().y));

		}
	}
	spawnPlayer();
}

void Scene_Play::spawnPlayer()
{
	m_player = m_entityManager.addEntity("player");
	m_player->addComponent<CAnimation>(m_game->assets().getAnimation("Stand"), true);
	m_player->addComponent<CTransform>(gridToMidPixel(2, 7, m_player));
	m_player->getComponent<CTransform>().scale.x = -1;
	m_player->addComponent<CBoundingBox>(Vec2(50,64));
	m_player->addComponent<CState>("falling");

	float jumpDuration = 0.6f;
	float gravity = 2 * m_maxJumpHeight / (float)pow(jumpDuration, 2);

	m_player->addComponent<CGravity>(gravity);
}

void Scene_Play::spawnBullet(std::shared_ptr<Entity> entity)
{
	
	auto& tranform = entity->getComponent<CTransform>();

	auto bullet = m_entityManager.addEntity("bullet");
	bullet->addComponent<CAnimation>(m_game->assets().getAnimation("Buster"), true);
	bullet->addComponent<CBoundingBox>(bullet->getComponent<CAnimation>().animation.getSize());
	bullet->addComponent<CLifespan>(90, m_currentFrame);
	bullet->addComponent<CTransform>(tranform.pos);
	bullet->getComponent<CTransform>().angle = tranform.angle;
	bullet->getComponent<CTransform>().pos = tranform.pos;
	bullet->getComponent<CTransform>().scale = tranform.scale;
	bullet->getComponent<CTransform>().velocity = Vec2(-64*10 * tranform.scale.x, 0);
}

void Scene_Play::update()
{
	//Add and remove entities
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

void Scene_Play::doAction(const Action& action)
{
	sDoAction(action);
}

void Scene_Play::sMovement(float deltaTime)
{	
	Vec2 playerVel(0, 0);
	std::string currState = m_player->getComponent<CState>().state;
	
	float maxJumpVelocity = -sqrtf(2 * m_player->getComponent<CGravity>().gravity * m_maxJumpHeight);
	if (m_player->getComponent<CInput>().up)
	{
		if (m_player->getComponent<CState>().state == "idle" || m_player->getComponent<CState>().state == "running")
		{
			m_player->getComponent<CTransform>().velocity.y = maxJumpVelocity;
		}
	}
	else
	{
		if (m_player->getComponent<CState>().state == "jumping")
		{
			m_player->getComponent<CTransform>().velocity.y = 0;
		}
	}
	if (m_player->getComponent<CInput>().left) {
		playerVel.x = -1;
		m_player->getComponent<CTransform>().scale.x = 1;
	}
	if (m_player->getComponent<CInput>().right) {
		playerVel.x = 1;
		m_player->getComponent<CTransform>().scale.x = -1;;
	}

	m_player->getComponent<CTransform>().velocity.x = playerVel.x * m_gridSize.x * 7;

	
	// Vertical states
	if (m_player->getComponent<CTransform>().prevPos.y == m_player->getComponent<CTransform>().pos.y)
	{
		if (m_player->getComponent<CTransform>().velocity.x != 0)
		{
			m_player->getComponent<CState>().state = "running";
		}
		else
		{
			m_player->getComponent<CState>().state = "idle";
		}
	}
	else if (m_player->getComponent<CTransform>().prevPos.y > m_player->getComponent<CTransform>().pos.y)
	{
		m_player->getComponent<CState>().state = "jumping";
	}
	else if (m_player->getComponent<CTransform>().prevPos.y < m_player->getComponent<CTransform>().pos.y)
	{
		m_player->getComponent<CState>().state = "falling";
	}

	if (currState != m_player->getComponent<CState>().state)
	{
		if (m_player->getComponent<CState>().state == "idle")
		{
			m_player->addComponent<CAnimation>(m_game->assets().getAnimation("Stand"), true);
		}
		else if (m_player->getComponent<CState>().state == "running")
		{
			m_player->getComponent<CAnimation>().animation = m_game->assets().getAnimation("Run");
		}
		else if (m_player->getComponent<CState>().state == "jumping" || m_player->getComponent<CState>().state == "falling")
		{
			m_player->getComponent<CAnimation>().animation = m_game->assets().getAnimation("Air");
		}
	}

	// Apply gravity
	m_player->getComponent<CTransform>().velocity.y += m_player->getComponent<CGravity>().gravity * deltaTime;
	// Clamp the player velocity to the gravity
	if (m_player->getComponent<CTransform>().velocity.y > m_player->getComponent<CGravity>().gravity)
		m_player->getComponent<CTransform>().velocity.y = m_player->getComponent<CGravity>().gravity;

	for (auto e : m_entityManager.getEntities())
	{
		e->getComponent<CTransform>().prevPos = e->getComponent<CTransform>().pos;
	}
	// Apply the velocity to the moveable entities
	const char* moveable[2] = {"bullet", "player"};
	for (int i = 0; i < 2; i++)
	{
		for (auto e : m_entityManager.getEntities(moveable[i]))
		{
			e->getComponent<CTransform>().pos += e->getComponent<CTransform>().velocity * deltaTime;
		}
	}

}

void Scene_Play::sLifespan()
{
	for (auto e : m_entityManager.getEntities("bullet"))
	{
		if (e->getComponent<CLifespan>().lifespan == 0)
		{
			e->destroy();
		}
		else
		{
			e->getComponent<CLifespan>().lifespan--;
		}
	}
}

void Scene_Play::sCollision()
{
	for (auto e : m_entityManager.getEntities("tile"))
	{
		if (e->hasComponent<CTransform>() && e->hasComponent<CBoundingBox>())
		{
			Vec2 overlap = Physics::GetOverlap(m_player, e);
			// Overlap
			if (overlap.x > 0.f && overlap.y > 0.f)
			{
				// Pole collision
				if (e->getComponent<CAnimation>().animation.getName() == "PoleTop" || e->getComponent<CAnimation>().animation.getName() == "Pole")
				{
					m_player->addComponent<CTransform>(gridToMidPixel(0, 7, m_player));
				}
				Vec2 previousOverlap = Physics::GetPreviousOverlap(m_player, e);
				// Horizontal Collision
				if (previousOverlap.y > 0)
				{
					// Left collision
					if (m_player->getComponent<CTransform>().prevPos.x > e->getComponent<CTransform>().pos.x)
					{
						m_player->getComponent<CTransform>().pos.x += overlap.x;
					}
					// Right collision
					else
					{
						m_player->getComponent<CTransform>().pos.x -= overlap.x;
					}
				}
				// Vertical Collision
				else if(previousOverlap.x > 0)
				{
					m_player->getComponent<CTransform>().velocity.y = 0;
					// Bottom collision
					if (m_player->getComponent<CTransform>().prevPos.y > e->getComponent<CTransform>().pos.y)
					{
						m_player->getComponent<CTransform>().pos.y += overlap.y;
						if (e->getComponent<CAnimation>().animation.getName() == "Question")
						{
							auto coin = m_entityManager.addEntity("coin");
							coin->addComponent<CAnimation>(m_game->assets().getAnimation("Coin"), false);
							Vec2 pos(e->getComponent<CTransform>().pos.x, e->getComponent<CTransform>().pos.y - 64);
							coin->addComponent<CTransform>(pos);

							e->addComponent<CAnimation>(m_game->assets().getAnimation("Question2"), true);

						}
					}
					// Top collision
					else
					{
						m_player->getComponent<CTransform>().pos.y -= overlap.y;
						m_player->getComponent<CTransform>().velocity.y = 0;
					}
				}
			}
			// Bullet tile collision
			for (auto b : m_entityManager.getEntities("bullet"))
			{
				if (!b->isActive()) { continue; }
				Vec2 bOverlap = Physics::GetOverlap(e, b);
				if (bOverlap.x > 0.f && bOverlap.y > 0.f)
				{
					b->destroy();
					if (e->getComponent<CAnimation>().animation.getName() == "Brick")
					{
						auto explosion = m_entityManager.addEntity("explosion");
						explosion->addComponent<CAnimation>(m_game->assets().getAnimation("Explosion"), false);
						explosion->addComponent<CTransform>(e->getComponent<CTransform>().pos);
						e->destroy();
					}
				}
			}
		}
	}
	// Prevent the player from leaving the right of the scene
	if (m_player->getComponent<CTransform>().pos.x < m_player->getComponent<CBoundingBox>().halfSize.x)
	{
		m_player->getComponent<CTransform>().pos.x = m_player->getComponent<CTransform>().prevPos.x;
	}
	// Reset the player when it falls out of the screen
	if (m_player->getComponent<CTransform>().pos.y > height())
	{
		m_player->addComponent<CTransform>(gridToMidPixel(0, 7, m_player));
	}

}

void Scene_Play::sDoAction(const Action& action)
{
	if (action.type() == "START")
	{
		if (action.name() == "TOGGLE_TEXTURE")			{ m_drawTextures = !m_drawTextures;}
		else if (action.name() == "TOGGLE_COLLISION")	{ m_drawCollision = !m_drawCollision; }
		else if (action.name() == "TOGGLE_GRID")		{ m_drawGrid = !m_drawGrid; }
		else if (action.name() == "PAUSE")				{setPaused(!m_paused); }
		else if (action.name() == "QUIT")				{ onEnd(); }
		else if (action.name() == "UP")					{ m_player->getComponent<CInput>().up = true; }
		else if (action.name() == "LEFT")				{ m_player->getComponent<CInput>().left = true; }
		else if (action.name() == "RIGHT")				{ m_player->getComponent<CInput>().right = true; }
		else if (action.name() == "DOWN")				{ m_player->getComponent<CInput>().down = true; }
		else if (action.name() == "SHOOT")
			 {
				 if (m_player->getComponent<CInput>().canShoot)
				 {
					 m_player->getComponent<CInput>().canShoot = false;
					 spawnBullet(m_player);
				 }
			 }
	}
	else if (action.type() == "END")
	{
		if (action.name() == "UP")			{ m_player->getComponent<CInput>().up = false; }
		else if (action.name() == "LEFT")	{ m_player->getComponent<CInput>().left = false; }
		else if (action.name() == "RIGHT")	{ m_player->getComponent<CInput>().right = false; }
		else if (action.name() == "DOWN")	{ m_player->getComponent<CInput>().down = false; }
		else if (action.name() == "SHOOT")	{ m_player->getComponent<CInput>().canShoot = true; }
	}
}

void Scene_Play::sAnimation()
{
	// TODO: Complete the Animaiton code

	// TODO: Set the animaiton of the player based on its CState 
	// TODO: for each entity with an animation, call entity->getComponent<CAnimation>().animation.update()
	//		 if the animation is not repeated, and it has ended, destroy the entity
	

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
			}
			e->getComponent<CAnimation>().animation.update();

		}
	}

}

void Scene_Play::onEnd()
{
	m_game->changeScene("MENU", std::make_shared<Scene_Menu>(m_game));
	m_game->window().setView(m_game->window().getDefaultView());
}

void Scene_Play::sRender()
{
	// color the background a darker color when paused
	if (!m_paused) { m_game->window().clear(sf::Color(0, 100, 255)); }
	else { m_game->window().clear(sf::Color(50, 50, 255)); }

	// set the viewport of the window to be centered on the player if it's far enough right
	auto& playerPos = m_player->getComponent<CTransform>().pos;
	float windowCenterX = std::max(m_game->window().getDefaultView().getSize().x / 2.0f, playerPos.x);
	sf::View view = m_game->window().getDefaultView();
	view.setCenter(windowCenterX, m_game->window().getDefaultView().getSize().y - view.getCenter().y);
	m_game->window().setView(view);


	// draw all Entity texture / animaitons
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
				rect.setFillColor(sf::Color(0, 0, 0, 0));
				rect.setOutlineColor(sf::Color(255, 255, 255, 255));
				rect.setOutlineThickness(1.f);
				m_game->window().draw(rect);
			}
		}
	}

	// draw the grid for debugging
	if (m_drawGrid)
	{
		float leftX = m_game->window().getView().getCenter().x - width() / 2;
		float rightX = leftX + width() + m_gridSize.x;
		float nextGridX = leftX - ((int)leftX % (int)m_gridSize.x);

		for (float x = nextGridX; x < rightX; x += m_gridSize.x)
		{
			drawLine(Vec2(x, 0), Vec2(x, (float)height()));
		}

		for (float y = 0; y < height(); y += m_gridSize.y)
		{
			drawLine(Vec2(leftX, height() - y), Vec2(rightX, height() - y));

			for (float x = nextGridX; x < rightX; x += m_gridSize.x)
			{
				std::string xCell = std::to_string((int)x / (int)m_gridSize.x);
				std::string yCell = std::to_string((int)y / (int)m_gridSize.y);
				m_gridText.setString("(" + xCell + "," + yCell + ")");
				m_gridText.setPosition(x + 3, height() - y - m_gridSize.y + 2);
				m_game->window().draw(m_gridText);
			}
		}
	}
	
	m_game->window().display();
}