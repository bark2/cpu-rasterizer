#pragma once

#include "vec3.hpp"
#include "StdAfx.h"

vec3 rgb2vec(COLORREF c);
COLORREF vec2rgb(const vec3& rgb);

class LightColor {
public:
	vec3 color;
	LightColor(COLORREF color) : color(rgb2vec(color)) {}
	LightColor(vec3 color = { 0, 0, 0 }) : color(color) {}

	COLORREF get_rgb() const {
		return vec2rgb(color);
	}

	LightColor operator+(LightColor& lc) {
		return color + lc.color;
	}

	void operator+=(LightColor& lc) {
		color += lc.color;
	}

	static LightColor get_reflected_color(const LightColor& light_c, const LightColor& obj_c);
};

LightColor operator*(const LightColor& lc, double d);
LightColor operator*(double d, const LightColor& lc);


typedef enum 
{
    LIGHT_ID_AMBIENT=-1,
    LIGHT_ID_1=0,
    LIGHT_ID_2,
    LIGHT_ID_3,
    LIGHT_ID_4,
    LIGHT_ID_5,
    LIGHT_ID_6,
    LIGHT_ID_7,
    LIGHT_ID_8,
    MAX_LIGHT
} LightID;

typedef  enum 
{
    LIGHT_TYPE_DIRECTIONAL,
    LIGHT_TYPE_POINT,
    LIGHT_TYPE_SPOT,
	LIGHT_TYPE_AMBIENT
} LightType;

typedef  enum 
{
    LIGHT_SPACE_VIEW,
    LIGHT_SPACE_LOCAL
} LightSpace;


class Light
{
public:

    //light enabled
    bool enabled;
    //type directional,point,spot
    LightType type;
    //local or view space
    LightSpace space;
    //color 0-1 RGB
	LightColor color;
    //position
	vec3 pos;
    //direction
	vec3 dir;
	//ambient coefficient
	double ambient_coeff;
	//diffuse coefficient
	double diffuse_coeff;
	//specular coefficient
	double specular_coeff;
	//spot light cone angle
	double cone_angle;


    
	Light() :
		enabled(false), type(LIGHT_TYPE_DIRECTIONAL), space(LIGHT_SPACE_VIEW),
		color(RGB(255, 255, 255)), pos(0, 0, 0), dir(0, 0, 0), diffuse_coeff(0.5), specular_coeff(0.5)
    {}

	Light(COLORREF color, vec3 pos, vec3 dir, LightType type = LIGHT_TYPE_DIRECTIONAL, LightSpace space = LIGHT_SPACE_VIEW, double diffuse_coeff = 0.5, double specular_coeff = 0.5) :
		enabled(true), type(type), space(space), color(color), pos(pos), dir(dir), diffuse_coeff(diffuse_coeff), specular_coeff(specular_coeff)
	{}
};
