#pragma once

#include <vector>
#include "vec3.hpp"
#include "mat4.h"
#include "QuickBMP.h"
#include "cg.hpp"

struct Pixel {
	int x;
	int y;
	COLORREF color;
	Pixel() : x(0), y(0), color(RGB(0, 0, 0)) {}
	Pixel(int x, int y, COLORREF color = RGB(0, 0, 0)) : x(x), y(y), color(color) {}
};

class Printer
{
	mat4 model;
	mat4 view;
	mat4 projection;

	bool isInBox(vec4 projected) {
		return (projected.x <= 1.0f && projected.x >= -1.0f &&
				projected.y <= 1.0f && projected.y >= -1.0f &&
				projected.z <= 1.0f && projected.z >= -1.0f);
	}

	Pixel screenCoord(const CRect& rect, vec4 projected_pos) {
		Pixel p;
		p.x = (int)round(rect.Width() * (projected_pos.x + 1.0f) / 2.0f);
		p.y = rect.Height() - (int)round(rect.Height() * (projected_pos.y + 1.0f) / 2.0f);
		return p;
	}

public:
	enum ProjectionMode { ORTHOGRAPHIC, PERSPECTIVE };
	void setProjection(ProjectionMode mode);

	Printer(ProjectionMode mode = ORTHOGRAPHIC) {
		setProjection(mode);
	}

	void drawObjVertices(QuickBMP& q_bmp, const CG_Object& obj);
	void drawObjWireframe(QuickBMP& q_bmp, const CG_Object& obj);
};
