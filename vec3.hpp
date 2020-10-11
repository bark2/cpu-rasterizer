#pragma once

#include <cmath>
#include <cassert>
#include "StdAfx.h"

struct vec3
{
    double x, y, z;

	vec3() : x(0), y(0), z(0) {}
    vec3(double x, double y, double z) : x(x), y(y), z(z) {}
    vec3(const vec3&) = default;

    const vec3& operator+() const { return *this; }
    vec3 operator-() const { return vec3(-x, -y, -z); }
    double operator*(const vec3& v) const {
		return x * v.x + y * v.y + z * v.z;
	}

    vec3& operator=(const vec3& v) = default;
    vec3& operator+=(const vec3& v2);
    vec3& operator-=(const vec3& v2);

//  vec3& operator*=(const vec3& v2);
    vec3& operator*=(const double t);
	vec3& operator/=(double t) {
		assert(t);
		x /= t;
		y /= t;
		z /= t;
		return *this;
	}
	vec3 operator/(double t) {
		vec3 result{ *this };
		return result /= t;
	}

	vec3 operator*(double t) const{
		vec3 result{ *this };
		return result *= t;
	}

    double Length() const { return sqrt(SquaredLength()); }
    double SquaredLength() const { return (x*x + y*y + z*z); }
	vec3 Normalize() {
		double len = Length();
		x /= len; y /= len; z /= len; return *this;
	}
};

vec3 UnitVector(vec3 v);
vec3 Cross(const vec3& v1, const vec3& v2);
double Dot(const vec3& v1, const vec3& v2);
vec3 Reflect(const vec3& light_d, const vec3& surface_normal);

inline vec3& vec3::operator*=(double t)
{
    x *= t;
    y *= t;
    z *= t;
    return *this;
}
inline bool operator==(const vec3& v1, const vec3& v2)
{
    return v1.x == v2.x && v1.y == v2.y && v1.z == v2.z;
}

inline bool operator==(const vec3& v1, vec3&& v2)
{
	return v1.x == v2.x && v1.y == v2.y && v1.z == v2.z;
}

inline bool operator!=(const vec3& v1, const vec3& v2)
{
    return !(v1 == v2);
}

inline vec3 operator+(const vec3& v1, const vec3& v2)
{
    return vec3(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z);
}

inline vec3 operator-(const vec3& v1, const vec3& v2)
{
    return vec3(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z);
}

inline vec3& vec3::operator+=(const vec3& v2)
{
    return *this = *(this) + v2;
}

inline vec3& vec3::operator-=(const vec3& v2)
{
    return *this = *(this) - v2;
}

inline bool operator<(const vec3& v1, const vec3& v2) {
	return v1.SquaredLength() < v2.SquaredLength();
}


vec3 operator*(double t, const vec3& v);