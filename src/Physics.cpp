#include "Physics.h"
#include "Components.h"
#include <iostream>

Vec2 Physics::GetOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b)
{
	// AABB Collision Detection
	
	
	float dx = abs(a->getComponent<CTransform>().pos.x - b->getComponent<CTransform>().pos.x);
	float xOverlap = a->getComponent<CBoundingBox>().halfSize.x + b->getComponent<CBoundingBox>().halfSize.x - dx;
	
	float dy = abs(a->getComponent<CTransform>().pos.y - b->getComponent<CTransform>().pos.y);
	float yOverlap = a->getComponent<CBoundingBox>().halfSize.y + b->getComponent<CBoundingBox>().halfSize.y - dy;
	Vec2 overlap(xOverlap, yOverlap);
	
	return overlap;
}

Vec2 Physics::GetPreviousOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b)
{
	float dx = abs(a->getComponent<CTransform>().prevPos.x - b->getComponent<CTransform>().prevPos.x);
	float xOverlap = a->getComponent<CBoundingBox>().halfSize.x + b->getComponent<CBoundingBox>().halfSize.x - dx;

	float dy = abs(a->getComponent<CTransform>().prevPos.y - b->getComponent<CTransform>().prevPos.y);
	float yOverlap = a->getComponent<CBoundingBox>().halfSize.y + b->getComponent<CBoundingBox>().halfSize.y - dy;

	Vec2 overlap(xOverlap, yOverlap);
	return overlap;
}

Intersect Physics::LineIntersect(const Vec2& a, const Vec2& b, const Vec2& c, const Vec2& d)
{
	Vec2 r = b - a;
	Vec2 s = d - c;
	float rxs = r * s;
	Vec2 cma = c - a;
	float t = (cma * s) / rxs;
	float u = (cma * r) / rxs;
	if (t >= 0 && t <= 1 && u >= 0 && u <= 1)
		return { true, Vec2(a.x + t * r.x, a.y + t * r.y) };
	return { false, Vec2(0, 0) };
}

bool Physics::EntityIntersect(const Vec2& a, const Vec2& b, std::shared_ptr<Entity> e)
{
	bool intersect = false;
	CBoundingBox& bBox = e->getComponent<CBoundingBox>();
	Vec2& position = e->getComponent<CTransform>().pos;

	Vec2 topL = Vec2(position.x - bBox.halfSize.x, position.y - bBox.halfSize.y);
	Vec2 topR = Vec2(position.x + bBox.halfSize.x, position.y - bBox.halfSize.y);
	Vec2 bottomL = Vec2(position.x - bBox.halfSize.x, position.y + bBox.halfSize.y);
	Vec2 bottomR = Vec2(position.x + bBox.halfSize.x, position.y + bBox.halfSize.y);

	if (LineIntersect(a, b, topL, topR).result) { intersect = true; }
	if (LineIntersect(a, b, topR, bottomR).result) { intersect = true; }
	if (LineIntersect(a, b, bottomL, bottomR).result) { intersect = true; }
	if (LineIntersect(a, b, topL, bottomL).result) { intersect = true; }

	return intersect;
}
