#include "Light.h"

vec3 rgb2vec(COLORREF c) {
	return { GetRValue(c) / 255.0, GetGValue(c) / 255.0, GetBValue(c) / 255.0 };
}

double rgb_clip(double c) {
	if (c < 0.0) return 0.0;
	if (c > 1.0) return 1.0;
	return c;
}

vec3 rgb_clip(const vec3& c) {
	return vec3{ rgb_clip(c.x), rgb_clip(c.y) , rgb_clip(c.z) };
}

COLORREF vec2rgb(const vec3& rgb) {
	vec3 rgb_255 = rgb_clip(rgb) * 255.0;
	return RGB((int)rgb_255.x, (int)rgb_255.y, (int)rgb_255.z);
}

LightColor operator*(const LightColor& lc, double d) {
	return LightColor(rgb_clip(lc.color * d));
}

LightColor operator*(double d, const LightColor& lc) {
	return LightColor(rgb_clip(lc.color * d));
}

LightColor LightColor::get_reflected_color(const LightColor& light_c, const LightColor& obj_c) {
	double r = obj_c.color.x * light_c.color.x;
	double g = obj_c.color.y * light_c.color.y;
	double b = obj_c.color.z * light_c.color.z;
	return rgb_clip(vec3{ r, g, b });
}