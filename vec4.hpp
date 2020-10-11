#ifndef VEC4_H
#define VEC4_H

#include <cmath>
#include "vec3.hpp"

struct vec4
{
	double x, y, z, w;

	vec4() = default;
	vec4(double x, double y, double z, double w = 1) : x(x), y(y), z(z), w(w) {}
	vec4(const vec4&) = default;
	vec4(const vec3& v, double w = 1) : x(v.x), y(v.y), z(v.z), w(w) {}

	const vec4 operator+() const { return *this; }
	const vec4 operator-() const { return vec4(-x, -y, -z, -w); }

	vec4& operator=(const vec4& v) = default;
/*	vec4& operator+=(const vec4& v2);
	vec4& operator-=(const vec4& v2);
	vec4& operator*=(const vec4& v2);
	vec4& operator*=(const double t);*/
	vec4& operator/=(double t) {
		x /= t;
		y /= t;
		z /= t;
		w /= t;		
		return *this;
	}

	explicit operator vec3() const { return vec3(x, y, z); }

	double Length() const { return sqrt(SquaredLength()); }
	double SquaredLength() const { return (x * x + y * y + z * z + w * w); }
	static double distance(const vec4& v1, const vec4& v2) { 
		return vec4(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z, v1.w - v2.w).Length();
	}
//	vec4 UnitVector();
	
	vec4 HomogenicDivision() {
		if (w == 0.0f) {
			throw "Can't HomogenicDivision. w == 0.";
			//return { 0, 0, 0, 0 };
		}
		x /= w; y /= w; z /= w; w = 1;
		return *this;
	}

	vec4 Normalize() {
		double l = Length();
		x /= l; y /= l; z /= l; w /= l;
		return *this;
	}
};

inline bool operator==(const vec4& v1, const vec4& v2)
{
	return v1.x == v2.x && v1.y == v2.y && v1.z == v2.z && v1.w == v2.w;
}

inline bool operator!=(const vec4& v1, const vec4& v2)
{
	return !(v1 == v2);
}

inline double operator*(const vec4& v1, const vec4& v2)
{
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z + v1.w * v2.w;
}

inline vec4 operator+(const vec4& v1, const vec4& v2) {
	return vec4(v1.x + v2.x, v1.y + v2.y, v1.z + v2.z, v1.w + v2.w);
}

inline vec4 operator-(const vec4& v1, const vec4& v2) {
	return vec4(v1.x - v2.x, v1.y - v2.y, v1.z - v2.z, v1.w - v2.w);
}

#endif