#pragma once

#ifndef CG_H
#define CG_H

#include "StdAfx.h"
#include <vector>
#include "vec3.hpp"
#include "vec4.hpp"
#include "mat4.h"
#include "ZBuffer.h"

class Scene;

extern Scene cg_scene;

namespace Helpers {
	int sign(int num);
	vec3 median(const std::vector<vec3>& points);
	void setLine(CDC * pDC, int x1, int y1, int x2, int y2, COLORREF c);
	void setLine_z(ZBuffer& zbuffer, vec3 v1, vec3 v2, COLORREF c);
	void draw_box(CDC* pDC, const Pipeline& pipe, COLORREF c, vec3 bbox_min, vec3 bbox_max);
	void draw_box_z(ZBuffer& zbuffer, const Pipeline& pipe, COLORREF c, vec3 bbox_min, vec3 bbox_max);
}

#endif