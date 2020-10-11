#pragma once

#include "StdAfx.h"
#include <vector>
#include "vec3.hpp"
#include "PngWrapper.h"

enum IMAGE_LAYOUT { IMAGE_STRECH, IMAGE_REPEAT };

class ZBuffer
{
	struct ZPixel {
		double depth;
		COLORREF color;
		bool valid; // valid == has been drawn to
	};

	typedef std::vector<ZPixel> ZColumn;
	typedef std::vector<ZColumn> ZArray;

	ZArray screen;

public:
	const unsigned int sw, sh;

	ZBuffer(unsigned int sw, unsigned int sh, COLORREF c = RGB(0, 0, 0))
		: screen(sw, ZColumn(sh, { -DBL_MAX, c, false })), sw(sw), sh(sh) {}
	
	bool set_bg_image(PngWrapper& bg, bool background_layout=IMAGE_REPEAT)
	{
		if(!bg.ReadPng()) 
			return false;
		float x_scale = sw / bg.GetWidth();
		float y_scale = sh / bg.GetHeight();
		for (unsigned int x = 0; x < sw; ++x) {
			for (unsigned int y = 0; y < sh; ++y) {
				switch (background_layout)
				{
				case IMAGE_STRECH:
					screen[x][y].color = bg.GetValue(static_cast<unsigned int>(x * x_scale), static_cast<unsigned int>(y * y_scale));
					break;
				case IMAGE_REPEAT:
					screen[x][y].color = bg.GetValue(x % bg.GetWidth(), y % bg.GetHeight());
					break;
				}
				screen[x][y].valid = true;
			}
		}
		return true;
	}

	void put_pixel(int x, int y, double z, COLORREF color) {
		if (x < 0 || x > sw - 1 || y < 0 || y > sh - 1)
			return;
		if (z > screen[x][y].depth) {
			screen[x][y].depth = z;
			screen[x][y].color = color;
			screen[x][y].valid = true;
		}
	}

	void draw2screen(CDC* pDC) const {
		for (unsigned int x = 0; x < sw; ++x) {
			for (unsigned int y = 0; y < sh; ++y) {
				if (screen[x][y].valid)
					pDC->SetPixel(x, y, screen[x][y].color);
			}
		}
	}

	void export_png(const char* name, int ew, int eh) const {
		PngWrapper png{ name , ew, eh };
		png.InitWritePng();
		float x_scale = ew / sw;
		float y_scale = eh / sh;
		for (unsigned int x = 0; x < sw; ++x) {
			for (unsigned int y = 0; y < sh; ++y) {
				if (screen[x][y].valid) { // not exporting true background
					int color = SET_RGB(GetRValue(screen[x][y].color),
						                GetGValue(screen[x][y].color), 
					                  	GetBValue(screen[x][y].color));
					png.SetValue(x * x_scale, y * y_scale, color);
				}
			}
		}
		png.WritePng();
	}
};