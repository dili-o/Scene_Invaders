#pragma once

#include "Animation.h"
#include "Assets.h"

class Component
{
public:
	bool has = false;
};

class CTransform : public Component
{
public:
	Vec2 pos		= { 0.f, 0.f }; // Center of rectangle
	Vec2 prevPos	= { 0.f, 0.f };
	Vec2 scale		= { 1.f, 1.f };
	Vec2 velocity	= { 0.f, 0.f };
	float angle		= 0.f;

	CTransform() {}
	CTransform(const Vec2& p)
		:pos(p) {}
	CTransform(const Vec2& p, const Vec2& vel, const Vec2& s, float a)
		:pos(p), prevPos(p), velocity(vel), scale(s), angle(a) {}

};


class CLifespan : public Component
{
public:
	int lifespan = 0;
	int frameCreated = 0;

	CLifespan() {}
	CLifespan(int duration, int frame)
		:lifespan(duration), frameCreated(frame) {}
};

class CInput : public Component
{
public:
	bool up = false;
	bool down = false;
	bool left = false;
	bool right = false;
	bool shoot = false;
	bool canShoot = true;
	bool canJump = true;

	CInput() {}
};

class CBoundingBox : public Component
{
public:
	Vec2 size;
	Vec2 halfSize;

	CBoundingBox() {}
	CBoundingBox(const Vec2& s)
		:size(s), halfSize(s.x / 2.f, s.y / 2.f) {}
};

class CAnimation : public Component
{
public:
	Animation animation;
	bool repeat = false;

	CAnimation() {}
	CAnimation(const Animation& animation, bool repeat)
		:animation(animation), repeat(repeat) {}
};

class CGravity : public Component
{
public:
	float gravity = 0;

	CGravity() {}
	CGravity(float g) : gravity(g) {}
};

class CState : public Component
{
public:
	std::string state = "";

	CState() {}
	CState(const std::string& s) : state(s) {}
};
