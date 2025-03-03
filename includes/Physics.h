#pragma once
#include "Vec2.h"
#include "Entity.h"
#include <memory>

struct Intersect
{
	bool result;
	Vec2 pos;
};

class Physics
{
public:
	static Vec2 GetOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b);

	static Vec2 GetPreviousOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b);

	static Intersect LineIntersect(const Vec2& a, const Vec2& b, const Vec2& c, const Vec2& d);

	static bool EntityIntersect(const Vec2& a, const Vec2& b, std::shared_ptr<Entity> e);
};

