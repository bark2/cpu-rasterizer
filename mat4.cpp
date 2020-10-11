#include "mat4.h"
#define _USE_MATH_DEFINES
#include <math.h>


mat4 mat4::Identity()
{
	return { {1, 0, 0, 0},
			 {0, 1, 0, 0},
			 {0, 0, 1, 0},
			 {0, 0, 0, 1} };
}

mat4 mat4::Ortho(double r, double t, double n, double f)
{
	mat4 result{ Transpose({
		1.0f / r, 0.0f, 0.0f, 0.0f,
		0.0f, 1.0f / t,    0.0f, 0.0f,
		0.0f, 0.0f, 2.0f / (f - n), (f + n) / (f - n),
		0, 0, 0, 1
	}) };
	return result;
}

#if 0
mat4 mat4::Persp(double fov_degrees, double aspect, double near, double far) {
	double fov_rad = M_PI * (fov_degrees / 180);
	double top = near * tan(fov_rad / 2);
	double bot = -top;
	double right = top * aspect;
	double left = -right;

	return mat4(
		2 * near / (right - left), 0, (right + left) / (right - left), 0,
		0, 2 * near / (top - bot), (top + bot) / (top - bot), 0,
		0, 0, (far + near) / (far - near), 2 * far * near / (far - near),
		0, 0, -1, 0);
}
#endif 

mat4 mat4::Persp_d(double n, double f) {
	mat4 result{ Transpose({
		n, 0, 0            , 0,
		0, n, 0            , 0,
		0, 0, 1.0f*(f + n) / (f - n)  , 2.0f*(f * n) / (f - n),
		0, 0, -1.0f     , 0
	}) };
	return result;
}


mat4 mat4::Translate(double tx, double ty, double tz)
{
	mat4 result{
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		tx, ty, tz, 1
	};
	return result;
}

mat4 mat4::Translate(vec3 v)
{
	return Translate(v.x, v.y, v.z);
}

mat4 mat4::Rotate(vec3 dir)
{
	dir.Normalize();

	double x_degrees = acos(dir.y) * 180.0 / M_PI;
	double y_degrees = acos(dir.z) * 180.0 / M_PI;
	double z_degrees = acos(dir.x) * 180.0 / M_PI;

	return Rotate(Axis::X, x_degrees) * Rotate(Axis::Y, y_degrees) *Rotate(Axis::Z, z_degrees);
}

mat4 mat4::Rotate(Axis fixed_axis, double degrees)
{
	double rad = M_PI * (degrees / 180);
	mat4 result;

	if (fixed_axis == Axis::X) {
		result = mat4(	1, 0, 0, 0,
						0, cos(rad), sin(rad), 0,
						0, -sin(rad), cos(rad), 0,
						0, 0, 0, 1	);
	} else if (fixed_axis == Axis::Y) {
		result = mat4(	cos(rad), 0, -sin(rad), 0,
						0, 1, 0, 0,
						sin(rad), 0, cos(rad), 0,
						0, 0, 0, 1	);
	} else if (fixed_axis == Axis::Z) {
		result = mat4(	cos(rad), sin(rad), 0, 0,
						-sin(rad), cos(rad), 0, 0,
						0, 0, 1, 0,
						0, 0, 0, 1);
	}
	return result;
}

mat4 mat4::Scale(double sx, double sy, double sz) {
	return mat4(	sx, 0, 0, 0,
					0, sy, 0, 0,
					0, 0, sz, 0,
					0, 0, 0,  1		);
}

mat4 Transpose(const mat4& m)
{
	return { {m.v1.x, m.v2.x, m.v3.x, m.v4.x},
			 {m.v1.y, m.v2.y, m.v3.y, m.v4.y},
			 {m.v1.z, m.v2.z, m.v3.z, m.v4.z},
			 {m.v1.w, m.v2.w, m.v3.w, m.v4.w} };
}

mat4 Transpose(mat4&& m)
{
	return { {m.v1.x, m.v2.x, m.v3.x, m.v4.x},
			 {m.v1.y, m.v2.y, m.v3.y, m.v4.y},
			 {m.v1.z, m.v2.z, m.v3.z, m.v4.z},
			 {m.v1.w, m.v2.w, m.v3.w, m.v4.w} };
}

vec3 pipeline_transform(const mat4& transofmation, const vec3& v) {
	return (vec3)(transofmation * vec4(v, 1)).HomogenicDivision();
}

vec3 pipeline_transform(const mat4& transofmation, const vec4& v) {
	return (vec3)(transofmation * v).HomogenicDivision();
}

vec3 pipeline_transform(const mat4& screen, const mat4& transofmation, const vec3& v) {
	return (vec3)(screen*((transofmation * vec4(v, 1)).HomogenicDivision()));
}

vec3 pipeline_transform(const mat4& screen, const mat4& transofmation, const vec4& v) {
	return (vec3)(screen*((transofmation * v).HomogenicDivision()));
}

vec3 identity_pipe(const vec4& v) { return (vec3)(v); };

double degrees(double rads) {
	return (rads / M_PI) * 180;
}