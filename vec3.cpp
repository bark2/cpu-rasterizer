#include "vec3.hpp"
#include <utility>

vec3 UnitVector(vec3 v)
{
	v.Normalize();
	return v;
}

vec3 Cross(const vec3& v1, const vec3& v2)
{
	return {
		v1.y*v2.z - v1.z*v2.y,
		v1.z*v2.x - v1.x*v2.z,
		v1.x*v2.y - v1.y*v2.x
	};
}

double Dot(const vec3& v1, const vec3& v2)
{
	return v1.x * v2.x + v1.y * v2.y + v1.z * v2.z;
}

vec3 Reflect(const vec3& light_d, const vec3& surface_normal) {
	// light_d is pointing into the surface
	double cos_alpha = light_d * surface_normal;
	return (light_d - surface_normal * 2 * cos_alpha).Normalize();
}


vec3 operator*(double t, const vec3& v) {
	return v * t;
}
