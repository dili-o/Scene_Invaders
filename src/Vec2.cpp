#include "Vec2.h"
#include <iostream>
#include <math.h>

Vec2::Vec2()
{
}

Vec2::Vec2(float xin, float yin)
	:x(xin), y(yin)
{
}

bool Vec2::operator==(const Vec2& rhs) const
{
	return (x == rhs.x && y == rhs.y);
}

bool Vec2::operator!=(const Vec2& rhs) const
{
	return !(x == rhs.x && y == rhs.y);
}

Vec2 Vec2::operator+(const Vec2& rhs) const
{
	return Vec2(x + rhs.x, y + rhs.y);
}

Vec2 Vec2::operator-(const Vec2& rhs) const
{
	return Vec2(x - rhs.x, y - rhs.y);
}

Vec2 Vec2::operator/(const float val) const
{
	return Vec2(x / val, y / val);
}

Vec2 Vec2::operator*(const float val) const
{
	return Vec2(x * val, y * val);
}

float Vec2::operator*(const Vec2 rhs) const
{
	return ((x * rhs.y) - (y * rhs.x));
}

void Vec2::operator+=(const Vec2& rhs)
{
	x += rhs.x;
	y += rhs.y;
}

void Vec2::operator-=(const Vec2& rhs)
{
	x -= rhs.x;
	y -= rhs.y;
}

void Vec2::operator/=(const float val)
{
	x /= val;
	y /= val;
}

void Vec2::operator*=(const float val)
{
	x *= val;
	y *= val;
}

float Vec2::dist(const Vec2& rhs) const
{
	return sqrtf((rhs.x - x) * (rhs.x - x) + (rhs.y - y) * (rhs.y - y));
}

Vec2 Vec2::normalise() const
{
	float length = sqrtf((x * x) + (y * y));
	if (length == 0)
	{
		return Vec2(0, 0);
	}
	return (Vec2(x, y) / length);
}
