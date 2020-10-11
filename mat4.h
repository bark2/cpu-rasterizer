#pragma once

#include <functional>
#include "vec4.hpp"
#include "vec3.hpp"

using Pipeline = std::function<vec3(const vec4&)>;

struct mat4
{
	// 4 column vectors
	vec4 v1, v2, v3, v4;

	mat4(const mat4& m) = default;
	mat4(vec4 v1 = vec4(1, 0, 0, 0),
		 vec4 v2 = vec4(0, 1, 0, 0),
		 vec4 v3 = vec4(0, 0, 1, 0),
		 vec4 v4 = vec4(0, 0, 0, 1))
		 : v1(v1), v2(v2), v3(v3), v4(v4) {}

	mat4(double v1x, double v1y, double v1z, double v1w,
		double v2x, double v2y, double v2z, double v2w,
		double v3x, double v3y, double v3z, double v3w,
		double v4x, double v4y, double v4z, double v4w)
		: v1(v1x, v1y, v1z, v1w)
		, v2(v2x, v2y, v2z, v2w)
		, v3(v3x, v3y, v3z, v3w)
		, v4(v4x, v4y, v4z, v4w)
	{}

	enum class Axis {X = 1, Y = 2, Z = 3};

	mat4& operator=(const mat4&) = default;

	static mat4 Identity();
	static mat4 Ortho(double r, double t, double n, double f);
	static mat4 Persp(double fov_degrees, double aspect, double near_v, double far_v);
	static mat4 Persp_d(double n, double f);
	static mat4 Translate(double tx, double ty, double tz);
	static mat4 Translate(vec3 v);
	static mat4 Scale(double sx, double sy, double sz);
	static mat4 mat4::Rotate(Axis fixed_axis, double degrees);
	static mat4 Rotate(vec3 dir);
};

mat4 Transpose(const mat4& m);
mat4 Transpose(mat4&& m);

inline vec4 operator*(const mat4& m, const vec4& v)
{
	mat4 t = Transpose(m);
	return { t.v1*v, t.v2*v, t.v3*v, t.v4*v };
}

inline mat4 operator*(const mat4& m1, const mat4& m2)
{
	mat4 tm1 = Transpose(m1);
	return { tm1.v1*m2.v1, tm1.v2*m2.v1, tm1.v3*m2.v1, tm1.v4*m2.v1,
			 tm1.v1*m2.v2, tm1.v2*m2.v2, tm1.v3*m2.v2, tm1.v4*m2.v2,
			 tm1.v1*m2.v3, tm1.v2*m2.v3, tm1.v3*m2.v3, tm1.v4*m2.v3,
			 tm1.v1*m2.v4, tm1.v2*m2.v4, tm1.v3*m2.v4, tm1.v4*m2.v4 };
}

vec3 pipeline_transform(const mat4& transofmation, const vec3& v);

vec3 pipeline_transform(const mat4& transofmation, const vec4& v);

vec3 pipeline_transform(const mat4& screen, const mat4& transofmation, const vec3& v);

vec3 pipeline_transform(const mat4& screen, const mat4& transofmation, const vec4& v);

vec3 identity_pipe(const vec4&);

double degrees(double rads);